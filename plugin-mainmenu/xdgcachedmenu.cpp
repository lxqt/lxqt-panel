/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#include "xdgcachedmenu.h"
#include <QAction>
#include <QIcon>
#include <QCursor>
#include <QToolTip>
#include <QList>
#include <QUrl>
#include <QDrag>
#include <QMouseEvent>
#include <QApplication>
#include <qtxdg/XdgDesktopFile>
#include <qtxdg/XdgIcon>

#include <QDebug>

XdgCachedMenuAction::XdgCachedMenuAction(MenuCacheItem* item, QObject* parent):
	QAction(parent),
	item_(menu_cache_item_ref(item))
{
	QString title = QString::fromUtf8(menu_cache_item_get_name(item));
	setText(title);
	QString comment = QString::fromUtf8(menu_cache_item_get_comment(item));
	setToolTip(comment);
	QIcon icon = XdgIcon::fromTheme(menu_cache_item_get_icon(item));
	setIcon(icon);
}

XdgCachedMenuAction::~XdgCachedMenuAction()
{
  if(item_)
    menu_cache_item_unref(item_);
}

XdgCachedMenu::XdgCachedMenu(MenuCache* menuCache, QWidget* parent): QMenu(parent)
{
    // qDebug() << "CREATE MENU FROM CACHE" << menuCache;
    connect(this, SIGNAL(hovered(QAction*)), SLOT(onItemHovered(QAction*)));
    MenuCacheDir* dir = menu_cache_get_root_dir(menuCache);
    addMenuItems(this, dir);
}

XdgCachedMenu::~XdgCachedMenu()
{
}

void XdgCachedMenu::addMenuItems(QMenu* menu, MenuCacheDir* dir)
{
  for(GSList* l = menu_cache_dir_get_children(dir); l; l = l->next)
  {
    MenuCacheItem* item = (MenuCacheItem*)l->data;
    MenuCacheType type = menu_cache_item_get_type(item);

    if(type == MENU_CACHE_TYPE_SEP)
    {
      menu->addSeparator();
      continue;
    }
    else
    {
      XdgCachedMenuAction* action = new XdgCachedMenuAction(item, menu);
      menu->addAction(action);
      if(type == MENU_CACHE_TYPE_APP)
        connect(action, SIGNAL(triggered(bool)), SLOT(onItemTrigerred()));
      else if(type == MENU_CACHE_TYPE_DIR)
      {
        XdgCachedMenu* submenu = new XdgCachedMenu(menu);
        connect(submenu, SIGNAL(hovered(QAction*)), SLOT(onItemHovered(QAction*)));
        action->setMenu(submenu);
        addMenuItems(submenu, (MenuCacheDir*)item);
      }
    }
  }
}

void XdgCachedMenu::onItemTrigerred()
{
    XdgCachedMenuAction* action = static_cast<XdgCachedMenuAction*>(sender());
    XdgDesktopFile df;
    char* desktop_file = menu_cache_item_get_file_path(action->item());
    df.load(desktop_file);
    g_free(desktop_file);
    df.startDetached();
}

// Qt does not show tooltips for menu items natively
// Let's do it manually here
void XdgCachedMenu::onItemHovered(QAction* action)
{
  QString tooltip = action->toolTip();
  if(!tooltip.isEmpty()) {
    QMenu* menu = static_cast<QMenu*>(sender());
    QToolTip::showText(QCursor::pos(), tooltip, menu);
  }
}

// taken from libqtxdg: XdgMenuWidget
bool XdgCachedMenu::event(QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        if (e->button() == Qt::LeftButton)
            mDragStartPosition = e->pos();
    }

    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        handleMouseMoveEvent(e);
    }

    return QMenu::event(event);
}

// taken from libqtxdg: XdgMenuWidget
void XdgCachedMenu::handleMouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    if ((event->pos() - mDragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    XdgCachedMenuAction *a = qobject_cast<XdgCachedMenuAction*>(actionAt(event->pos()));
    if (!a)
        return;

    QList<QUrl> urls;
    char* desktop_file = menu_cache_item_get_file_path(a->item());
    urls << QUrl(desktop_file);
    g_free(desktop_file);

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(urls);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction | Qt::LinkAction);
}
