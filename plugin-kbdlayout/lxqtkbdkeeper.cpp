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

#include <QDebug>
#include "lxqtkbdkeeper.h"

#include <QX11Info>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>


LxQtKbdLayoutKeeper::LxQtKbdLayoutKeeper(LxQtKbdInfo & info):
    m_info(info)
{}

void LxQtKbdLayoutKeeper::updateInfo(int group)
{
    m_info.setCurrentGroup(group);
    emit changed();
}

void LxQtKbdLayoutKeeper::checkLayout()
{}


Window LxQtKbdLayoutKeeper::activeWindowId()
{
    Display* disp = QX11Info::display();
    Window wnd;
    int revert;
    XGetInputFocus(disp, &wnd, &revert);
    return wnd;
}

QString LxQtKbdLayoutKeeper::windowClassName(Window wndId)
{
    XClassHint hint;
    if (XGetClassHint(QX11Info::display(), wndId, &hint) != 0){
        QString winName = hint.res_class;
        if (hint.res_class)
            XFree(hint.res_class);
        if (hint.res_name)
            XFree(hint.res_name);
        return winName;
    }
    return "";
}

Window LxQtKbdLayoutKeeper::parentWindow(Window wndId)
{
    Window root, parent,* child;
    unsigned int n;
    if (XQueryTree(QX11Info::display(), wndId, &root, &parent, &child, &n)){
        return parent;
    }
    return 0;
}

QString LxQtKbdLayoutKeeper::windowAppName(Window wndId)
{
    QString win = windowClassName(wndId);
    Window getWin = wndId;
    while (win.isEmpty()){
        getWin = parentWindow(getWin);
        if (!getWin)
            return "NONE";

        win = windowClassName(getWin);
    }
    return win;
}

//--------------------------------------------------------------------------------------------------

LxQtKbdLayoutAppKeeper::LxQtKbdLayoutAppKeeper(LxQtKbdInfo & info):
    LxQtKbdLayoutKeeper(info),
    m_active(windowAppName(activeWindowId()))
{}

void LxQtKbdLayoutAppKeeper::updateInfo(int group)
{
    QString app = windowAppName(activeWindowId());

    if (m_active == app){
        m_mapping[app] = group;
        m_info.setCurrentGroup(group);
    } else {
        if (!m_mapping.contains(app))
            m_mapping.insert(app, 0);

        XkbLockGroup(QX11Info::display(), XkbUseCoreKbd, m_mapping[app]);
        m_active = app;
        m_info.setCurrentGroup(m_mapping[app]);
    }
    emit changed();
}

void LxQtKbdLayoutAppKeeper::checkLayout()
{
    QString app = windowAppName(activeWindowId());

    if( m_active != app) {
        if (!m_mapping.contains(app))
            m_mapping.insert(app, 0);

        XkbLockGroup(QX11Info::display(), XkbUseCoreKbd, m_mapping[app]);
        m_active = app;
        m_info.setCurrentGroup(m_mapping[app]);
        emit changed();
    }
}

//--------------------------------------------------------------------------------------------------

LxQtKbdLayoutWinKeeper::LxQtKbdLayoutWinKeeper(LxQtKbdInfo & info):
    LxQtKbdLayoutKeeper(info),
    m_active(activeWindowId())
{}


void LxQtKbdLayoutWinKeeper::updateInfo(int group)
{
    Window win = activeWindowId();

    if (m_active == win){
        m_mapping[win] = group;
        m_info.setCurrentGroup(group);
    } else {
        if (!m_mapping.contains(win))
            m_mapping.insert(win, 0);
        XkbLockGroup(QX11Info::display(), XkbUseCoreKbd, m_mapping[win]);
        m_active = win;
        m_info.setCurrentGroup(m_mapping[win]);
    }
    emit changed();
}

void LxQtKbdLayoutWinKeeper::checkLayout()
{
    Window win = activeWindowId();

    if( m_active != win) {
        if (!m_mapping.contains(win))
            m_mapping.insert(win, 0);
        XkbLockGroup(QX11Info::display(), XkbUseCoreKbd, m_mapping[win]);
        m_active = win;
        m_info.setCurrentGroup(m_mapping[win]);
        emit changed();
    }
}
