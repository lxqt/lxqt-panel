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

#if !defined(ACTION_VIEW_H)
#define ACTION_VIEW_H

#include <QListView>
#include <QPoint>

class QStandardItemModel;

//==============================

#include <QSortFilterProxyModel>
class FilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FilterProxyModel(QObject* parent = nullptr);
    virtual ~FilterProxyModel();

    void setfilterString(const QString &str);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

#ifndef HAVE_MENU_CACHE
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
#endif


private:
    QString filterStr_;
};

//==============================
class ActionView : public QListView
{
    Q_OBJECT
public:
    enum Role
    {
        ActionRole = Qt::UserRole
            , FilterRole = ActionRole + 1
    };

public:
    ActionView(QWidget * parent = nullptr);

    /*! \brief Remove all items from model
     */
    void clear();
    /*! \brief Add action proxy to the underlying model
     */
    void addAction(QAction * action);
    /*! \brief Check if action already exists in the view/model.
     *
     * \note The equality is evaluated just on text() & toolTip()
     */
    bool existsAction(QAction const * action) const;
    /*! \brief Fill the view with all actions from \param menu
     */
    void fillActions(QMenu * menu);
    /*! \brief Sets the filter for entries to be presented
     */
    void setFilter(QString const & filter);
    /*! \brief Set the maximum number of items/results to show
     */
    void setMaxItemsToShow(int max);
    /*! \brief Set the maximum width of item to show
     */
    void setMaxItemWidth(int max);

public slots:
    /*! \brief Trigger action on currently active item
     */
    void activateCurrent();

protected:
    virtual QSize viewportSizeHint() const override;
    virtual QSize minimumSizeHint() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void requestShowHideMenu();

private slots:
    void onActivated(QModelIndex const & index);
    void onActionDestroyed();

private:
    void fillActionsRecursive(QMenu * menu);

private:
    QStandardItemModel * mModel;
    QPoint mDragStartPosition;
#ifdef HAVE_MENU_CACHE
    QSortFilterProxyModel * mProxy;
#else
    FilterProxyModel * mProxy;
#endif
    int mMaxItemsToShow;
};

#endif //ACTION_VIEW_H
