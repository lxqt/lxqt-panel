#include "lxqttaskbarproxymodel.h"

#include "../panel/backends/ilxqttaskbarabstractbackend.h"

#include <QIcon>

LXQtTaskBarProxyModel::LXQtTaskBarProxyModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_backend(nullptr)
    , m_groupByWindowClass(false)
{

}

int LXQtTaskBarProxyModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_items.count();
}

QVariant LXQtTaskBarProxyModel::data(const QModelIndex &idx, int role) const
{
    if (!idx.isValid() || idx.row() >= m_items.count())
        return QVariant();

    const LXQtTaskBarProxyModelItem& item = m_items.at(idx.row());

    switch (role)
    {
    case Qt::DisplayRole:
        return item.windows.count() == 1 ? item.windows.first().title : item.windowClass;
    default:
        break;
    }

    return QVariant();
}

QIcon LXQtTaskBarProxyModel::getWindowIcon(int itemRow, int windowIdxInGroup, int devicePixels) const
{
    if(!m_backend || itemRow < 0 || itemRow >= m_items.size() || windowIdxInGroup < 0)
        return QIcon();

    const LXQtTaskBarProxyModelItem& item = m_items.at(itemRow);
    if(windowIdxInGroup >= item.windows.size())
        return QIcon();

    const LXQtTaskBarProxyModelWindow& window = item.windows.at(windowIdxInGroup);
    return m_backend->getApplicationIcon(window.windowId, devicePixels);
}

void LXQtTaskBarProxyModel::onWindowAdded(WId windowId)
{
    QString windowClass = m_backend->getWindowClass(windowId);
    bool willAddRow = !m_groupByWindowClass || !hasWindowClass(windowClass);

    if(willAddRow)
    {
        const int row = m_items.count();
        beginInsertRows(QModelIndex(), row, row);
    }

    addWindow_internal(windowId);

    if(willAddRow)
        endInsertRows();
}

void LXQtTaskBarProxyModel::onWindowRemoved(WId windowId)
{
    int row = -1;
    int windowIdxInGroup = -1;
    for(int i = 0; i < m_items.count(); i++)
    {
        windowIdxInGroup = m_items.at(i).indexOfWindow(windowId);
        if(windowIdxInGroup != -1)
        {
            row = i;
            break;
        }
    }

    if(row == -1)
        return;

    LXQtTaskBarProxyModelItem& item = m_items[row];
    item.windows.removeAt(windowIdxInGroup);

    if(item.windows.isEmpty())
    {
        // Remove the group
        beginRemoveRows(QModelIndex(), row, row);
        m_items.removeAt(row);
        endRemoveRows();
    }
}

void LXQtTaskBarProxyModel::onWindowPropertyChanged(WId windowId, int prop)
{
    int row = -1;
    int windowIdxInGroup = -1;
    for(int i = 0; i < m_items.count(); i++)
    {
        windowIdxInGroup = m_items.at(i).indexOfWindow(windowId);
        if(windowIdxInGroup != -1)
        {
            row = i;
            break;
        }
    }

    if(row == -1)
        return;

    LXQtTaskBarProxyModelItem& item = m_items[row];
    LXQtTaskBarProxyModelWindow& window = item.windows[windowIdxInGroup];

    switch (LXQtTaskBarWindowProperty(prop))
    {
    case LXQtTaskBarWindowProperty::Title:
        window.title = m_backend->getWindowTitle(window.windowId);
        break;

    case LXQtTaskBarWindowProperty::Urgency:
        window.demandsAttention = m_backend->applicationDemandsAttention(window.windowId);
        break;

    case LXQtTaskBarWindowProperty::WindowClass:
    {
        // If window class is changed, window won't be part of same group
        //TODO: optimize
        onWindowRemoved(windowId);
        onWindowAdded(windowId);
    }

    default:
        break;
    }

    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx, {Qt::DisplayRole, Qt::DecorationRole});
}

void LXQtTaskBarProxyModel::addWindow_internal(WId windowId)
{
    LXQtTaskBarProxyModelWindow window;
    window.windowId = windowId;
    window.title = m_backend->getWindowTitle(window.windowId);
    window.demandsAttention = m_backend->applicationDemandsAttention(window.windowId);

    QString windowClass = m_backend->getWindowClass(window.windowId);

    int row = -1;
    if(m_groupByWindowClass)
    {
        // Find existing group
        for(int i = 0; i < m_items.count(); i++)
        {
            if(m_items.at(i).windowClass == windowClass)
            {
                row = i;
                break;
            }
        }
    }

    if(row == -1)
    {
        // Create new group
        LXQtTaskBarProxyModelItem item;
        item.windowClass = windowClass;
        m_items.append(item);
        row = m_items.size() - 1;
    }

    // Add window to group
    LXQtTaskBarProxyModelItem& item = m_items[row];
    item.windows.append(window);
}

bool LXQtTaskBarProxyModel::groupByWindowClass() const
{
    return m_groupByWindowClass;
}

void LXQtTaskBarProxyModel::setGroupByWindowClass(bool newGroupByWindowClass)
{
    if(m_groupByWindowClass == newGroupByWindowClass)
        return;

    m_groupByWindowClass = newGroupByWindowClass;

    if(m_backend && !m_items.isEmpty())
    {
        beginResetModel();

        m_items.clear();

        // Reload current windows
        const QVector<WId> windows = m_backend->getCurrentWindows();
        m_items.reserve(windows.size());

        for(WId windowId : windows)
            onWindowAdded(windowId);

        m_items.squeeze();

        endResetModel();
    }

}

ILXQtTaskbarAbstractBackend *LXQtTaskBarProxyModel::backend() const
{
    return m_backend;
}

void LXQtTaskBarProxyModel::setBackend(ILXQtTaskbarAbstractBackend *newBackend)
{
    beginResetModel();

    m_items.clear();

    if(m_backend)
    {
        disconnect(m_backend, &ILXQtTaskbarAbstractBackend::windowAdded,
                   this, &LXQtTaskBarProxyModel::onWindowAdded);
        disconnect(m_backend, &ILXQtTaskbarAbstractBackend::windowRemoved,
                   this, &LXQtTaskBarProxyModel::onWindowRemoved);
        disconnect(m_backend, &ILXQtTaskbarAbstractBackend::windowPropertyChanged,
                   this, &LXQtTaskBarProxyModel::onWindowPropertyChanged);
    }

    m_backend = newBackend;

    if(m_backend)
    {
        connect(m_backend, &ILXQtTaskbarAbstractBackend::windowAdded,
                this, &LXQtTaskBarProxyModel::onWindowAdded);
        connect(m_backend, &ILXQtTaskbarAbstractBackend::windowRemoved,
                this, &LXQtTaskBarProxyModel::onWindowRemoved);
        connect(m_backend, &ILXQtTaskbarAbstractBackend::windowPropertyChanged,
                this, &LXQtTaskBarProxyModel::onWindowPropertyChanged);

        // Reload current windows
        const QVector<WId> windows = m_backend->getCurrentWindows();
        m_items.reserve(windows.size());

        for(WId windowId : windows)
            onWindowAdded(windowId);

        m_items.squeeze();
    }

    m_items.squeeze();

    endResetModel();
}
