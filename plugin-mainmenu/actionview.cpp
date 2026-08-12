/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2016 LXQt team
 * Authors:
 *   Palo Kisa <palo.kisa@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "actionview.h"
#include <XdgAction>

#include <QAction>
#include <QWidgetAction>
#include <QMenu>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QProxyStyle>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QUrl>

#include <algorithm>

//==============================
FilterProxyModel::FilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortLocaleAware(true);
}

FilterProxyModel::~FilterProxyModel() = default;

bool FilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const {
    if (filterStr_.isEmpty())
        return true;
    if (QStandardItemModel* srcModel = static_cast<QStandardItemModel*>(sourceModel())) {
        QModelIndex index = srcModel->index(source_row, 0, source_parent);
        if (QStandardItem * item = srcModel->itemFromIndex(index)) {
            XdgAction * action = qobject_cast<XdgAction *>(qvariant_cast<QAction *>(item->data(ActionView::ActionRole)));
            if (action) {
                const XdgDesktopFile& df = action->desktopFile();
                if (df.name().contains(filterStr_, filterCaseSensitivity()))
                    return true;
                QStringList list = df.expandExecString();
                if (!list.isEmpty()) {
                    if (list.at(0).contains(filterStr_, filterCaseSensitivity()))
                        return true;
                }
            }
        }
    }
    return false;
}
//==============================
namespace
{
    class SingleActivateStyle : public QProxyStyle
    {
    public:
        using QProxyStyle::QProxyStyle;
        int styleHint(StyleHint hint, const QStyleOption * option = nullptr, const QWidget * widget = nullptr, QStyleHintReturn * returnData = nullptr) const override
        {
            if(hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
                return 1;
            return QProxyStyle::styleHint(hint, option, widget, returnData);

        }
    };

    class DelayedIconDelegate : public QStyledItemDelegate
    {
    public:
        DelayedIconDelegate(QObject * parent = nullptr)
            : QStyledItemDelegate(parent)
        {
        }

        void setMaxItemWidth(int max)
        {
            mMaxItemWidth = max;
        }

        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
        {
            QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
            //the XdgCachedMenuAction/XdgAction does load the icon upon showing its menu
            if (icon.isNull())
            {
                XdgAction * action = qobject_cast<XdgAction *>(qvariant_cast<QAction *>(index.data(ActionView::ActionRole)));
                if (action != nullptr)
                {
                  action->updateIcon();
                  const_cast<QAbstractItemModel *>(index.model())->setData(index, action->icon(), Qt::DecorationRole);
                }
            }
            QSize s = QStyledItemDelegate::sizeHint(option, index);
            s.setWidth(std::min(mMaxItemWidth, s.width()));
            return s;
        }
    private:
        int mMaxItemWidth = 300;
    };

}
//==============================
ActionView::ActionView(QWidget * parent /*= nullptr*/)
    : QListView(parent)
    , mModel{new QStandardItemModel{this}}
    , mProxy{new FilterProxyModel{this}}
    , mMaxItemsToShow(10)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSizeAdjustPolicy(AdjustToContents);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSelectionBehavior(SelectRows);
    setSelectionMode(SingleSelection);

    SingleActivateStyle * s = new SingleActivateStyle;
    s->setParent(this);
    setStyle(s);
    mProxy->setSourceModel(mModel);
    mProxy->setDynamicSortFilter(true);
    mProxy->setFilterRole(FilterRole);
    mProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mProxy->sort(0);
    {
        std::unique_ptr<QItemSelectionModel> guard{selectionModel()};
        setModel(mProxy);
    }
    {
        std::unique_ptr<QAbstractItemDelegate> guard{itemDelegate()};
        setItemDelegate(new DelayedIconDelegate{this});
    }
    connect(this, &QAbstractItemView::activated, this, &ActionView::onActivated);
}

void ActionView::ActionView::clear()
{
    for (int i = mModel->rowCount() - 1; i >= 0; --i)
    {
        mModel->removeRow(i);
    }
}

void ActionView::addAction(QAction * action)
{
    QStandardItem * item = new QStandardItem;
    item->setData(QVariant::fromValue<QAction *>(action), ActionRole);
    item->setFont(action->font());
    //Note: XdgCachedMenuAction has delayed icon loading... we are loading the icon
    //in QStyledItemDelegate:sizeHint if necessary
    item->setIcon(action->icon());
    item->setText(action->text());
    item->setToolTip(action->toolTip());
    QString all = action->text();
    all += QLatin1Char('\n');
    all += action->toolTip();
    item->setData(all, FilterRole);

    mModel->appendRow(item);
    connect(action, &QObject::destroyed, this, &ActionView::onActionDestroyed);
}

bool ActionView::existsAction(QAction const * action) const
{
    bool exists = false;
    for (int row = mModel->rowCount() - 1; 0 <= row; --row)
    {
        auto a = qobject_cast<XdgAction const *>(action);
        auto b = qobject_cast<XdgAction const *>(mModel->item(row)->data(ActionRole).value<QAction*>());
        if (a->desktopFile().fileName() == b->desktopFile().fileName())
        {
            exists = true;
            break;
        }

    }
    return exists;
}

void ActionView::fillActions(QMenu * menu)
{
    clear();
    fillActionsRecursive(menu);
}

void ActionView::setFilter(QString const & filter)
{
    mProxy->setFilterString(filter);
    const int count = mProxy->rowCount();
    if (0 < count)
    {
        if (count > mMaxItemsToShow)
        {
            setCurrentIndex(mProxy->index(mMaxItemsToShow - 1, 0));
            verticalScrollBar()->triggerAction(QScrollBar::SliderToMinimum);
        } else
        {
            setCurrentIndex(mProxy->index(count - 1, 0));
        }
    }
}

void ActionView::setMaxItemsToShow(int max)
{
    mMaxItemsToShow = max;
}

void ActionView::setMaxItemWidth(int max)
{
    dynamic_cast<DelayedIconDelegate *>(itemDelegate())->setMaxItemWidth(max);
}

void ActionView::activateCurrent()
{
    QModelIndex const index = currentIndex();
    if (index.isValid())
        emit activated(index);
}

QSize ActionView::viewportSizeHint() const
{
    const int count = mProxy->rowCount();
    QSize s{0, 0};
    if (0 < count)
    {
        const bool scrollable = mMaxItemsToShow < count;
        s.setWidth(sizeHintForColumn(0) + (scrollable ? verticalScrollBar()->sizeHint().width() : 0));
        s.setHeight(sizeHintForRow(0) * (scrollable ? mMaxItemsToShow : count));
    }
    return s;
}

QSize ActionView::minimumSizeHint() const
{
    return QSize{0, 0};
}

void ActionView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        mDragStartPosition = event->position().toPoint();

    QListView::mousePressEvent(event);
}

void ActionView::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->position().toPoint() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    XdgAction *a = qobject_cast<XdgAction*>(indexAt(mDragStartPosition).data(ActionView::ActionRole).value<QAction*>());
    if (!a)
        return;

    QList<QUrl> urls;
    urls << QUrl::fromLocalFile(a->desktopFile().fileName());

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(urls);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::LinkAction);
    emit requestShowHideMenu();
}

void ActionView::onActivated(QModelIndex const & index)
{
    QAction * action = qvariant_cast<QAction *>(model()->data(index, ActionRole));
    Q_ASSERT(nullptr != action);
    action->trigger();
}

void ActionView::onActionDestroyed()
{
    QObject * const action = sender();
    Q_ASSERT(nullptr != action);
    for (int i = mModel->rowCount() - 1; 0 <= i; --i)
    {
        QStandardItem * item = mModel->item(i);
        if (action == item->data(ActionRole).value<QObject *>())
        {
            mModel->removeRow(i);
            break;
        }
    }
}

void ActionView::fillActionsRecursive(QMenu * menu)
{
    const auto actions = menu->actions();
    for (auto const & action : actions)
    {
        if (QMenu * sub_menu = action->menu())
        {
            fillActionsRecursive(sub_menu); //recursion
        } else if (nullptr == qobject_cast<QWidgetAction* >(action)
                && !action->isSeparator())
        {
            //real menu action -> app
            if (!existsAction(action))
                addAction(action);
        }
    }
}

