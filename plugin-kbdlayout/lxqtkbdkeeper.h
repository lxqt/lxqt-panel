/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Jes <zjesclean.gmail@gmail.com>
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

#ifndef _LXQTKBDKEEPER_H_
#define _LXQTKBDKEEPER_H_

#include <QObject>
#include <QHash>
#include <QString>
#include <X11/XKBlib.h>
#include "lxqtkbdinfo.h"

class LxQtKbdLayoutKeeper: public QObject
{
    Q_OBJECT
public:
    LxQtKbdLayoutKeeper(LxQtKbdInfo & info);

    virtual void updateInfo(int group);
    virtual void checkLayout();

public:
    static Window activeWindowId();
    static QString windowClassName(Window wndId);
    static Window parentWindow(Window wndId);
    static QString windowAppName(Window wndId);

signals:
    void changed();
protected:
    LxQtKbdInfo & m_info;
};

class LxQtKbdLayoutAppKeeper: public LxQtKbdLayoutKeeper
{
public:
    LxQtKbdLayoutAppKeeper(LxQtKbdInfo & info);

    virtual void updateInfo(int group);
    virtual void checkLayout();

private:
    QHash<QString, int> m_mapping;
    QString             m_active;
};

class LxQtKbdLayoutWinKeeper: public LxQtKbdLayoutKeeper
{
public:
    LxQtKbdLayoutWinKeeper(LxQtKbdInfo & info);

    virtual void updateInfo(int group);
    virtual void checkLayout();

private:
    QHash<Window, int> m_mapping;
    Window             m_active;
};


#endif
