#ifndef LXQTTASKBARPROXYMODEL_H
#define LXQTTASKBARPROXYMODEL_H

#include <QAbstractListModel>
#include <QVector>

#include "../panel/backends/lxqttaskbartypes.h"

class ILXQtTaskbarAbstractBackend;

class LXQtTaskBarProxyModelWindow
{
public:
    LXQtTaskBarProxyModelWindow() = default;

    WId windowId;
    QString title;
    bool demandsAttention = false;
};

// Single window or group
class LXQtTaskBarProxyModelItem
{
public:
    LXQtTaskBarProxyModelItem() = default;

    QVector<LXQtTaskBarProxyModelWindow> windows;
    QString windowClass;

    inline bool demandsAttention() const
    {
        for(const LXQtTaskBarProxyModelWindow& w : windows)
        {
            if(w.demandsAttention)
                return true;
        }

        return false;
    }

    int indexOfWindow(WId windowId) const
    {
        for(int i = 0; i < windows.size(); i++)
        {
            if(windows.at(i).windowId == windowId)
                return i;
        }

        return -1;
    }
};

class LXQtTaskBarProxyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LXQtTaskBarProxyModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;

    QIcon getWindowIcon(int itemRow, int windowIdxInGroup, int devicePixels) const;

    ILXQtTaskbarAbstractBackend *backend() const;
    void setBackend(ILXQtTaskbarAbstractBackend *newBackend);

    bool groupByWindowClass() const;
    void setGroupByWindowClass(bool newGroupByWindowClass);

private slots:
    void onWindowAdded(WId windowId);
    void onWindowRemoved(WId windowId);
    void onWindowPropertyChanged(WId windowId, int prop);

private:
    void addWindow_internal(WId windowId);

    inline bool hasWindowClass(const QString& windowClass) const
    {
        for(const LXQtTaskBarProxyModelItem& item : m_items)
        {
            if(item.windowClass == windowClass)
                return true;
        }

        return false;
    }

private:
    ILXQtTaskbarAbstractBackend *m_backend;

    QVector<LXQtTaskBarProxyModelItem> m_items;

    bool m_groupByWindowClass;
};

#endif // LXQTTASKBARPROXYMODEL_H
