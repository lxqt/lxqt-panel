/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#pragma once

#include <QMenu>
#include "lxqtpanelglobals.h"

class LXQT_PANEL_API PopupMenu: public QMenu
{
public:
    explicit PopupMenu(QWidget *parent = nullptr): QMenu(parent) {}
    explicit PopupMenu(const QString &title, QWidget *parent = nullptr): QMenu(title, parent) {}

    QAction* addTitle(const QIcon &icon, const QString &text);
    QAction* addTitle(const QString &text);

    bool eventFilter(QObject *object, QEvent *event);

protected:
    virtual void keyPressEvent(QKeyEvent* e);
};
