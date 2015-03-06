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

#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QX11Info>
#include <X11/XKBlib.h>

#include "lxqtkbdinfo.h"

bool LxQtKbdInfo::readKeyboardInfo()
{
    m_keyboardInfo.clear();

    // load current settings from the output of setxkbmap command
    QProcess setxkbmap;
    setxkbmap.start(QLatin1String("setxkbmap -query -verbose 5"));
    setxkbmap.waitForFinished();

    if(setxkbmap.exitStatus() != QProcess::NormalExit)
        return false;

    QStringList layouts;
    while(!setxkbmap.atEnd()) {
        QString line = setxkbmap.readLine().trimmed();
        if(line.startsWith("layout:")){
            layouts = line.mid(7).trimmed().split(',');
            break;
        }
    }

    setxkbmap.close();

    if (layouts.size() < 2)
        return false;

    for(const QString & lay: layouts){
        m_keyboardInfo.append({lay, ""});
    }

    readRules();
    readCurrent();

    return true;
}

void LxQtKbdInfo::readRules()
{
    if (!QFile::exists("/usr/share/X11/xkb/rules/base.lst"))
        return;

    QFile file(QLatin1String("/usr/share/X11/xkb/rules/base.lst"));
    if(!file.open(QIODevice::ReadOnly))
        return;

    bool read = false;
    while(!file.atEnd()) {
        QString line = file.readLine().trimmed();
        if (line.startsWith('!')){
            if (read)
                break;
            read = (line == "! layout");
            continue;
        }

        if (!read)
            continue;

        QStringList it = line.split("  ", QString::SkipEmptyParts);
        if (!it.size())
            continue;

        for(Info & info: m_keyboardInfo){
            if (info.sym == it[0])
                info.name = it[1];
        }
    }

    file.close();
}

void LxQtKbdInfo::readCurrent()
{
    XkbDescRec * kbd = XkbAllocKeyboard();
    if (!kbd)
        return;

    kbd->dpy = QX11Info::display();
    if (XkbGetControls(kbd->dpy, XkbAllControlsMask, kbd) == Success) {
        uint ngroups = kbd->ctrls->num_groups;
        if (ngroups >= 1){
            XkbStateRec state;
            if (XkbGetState(kbd->dpy, XkbUseCoreKbd, &state) == Success) {
                m_current = state.group;
            }
        }
    }
    XkbFreeKeyboard(kbd, 0, True);
}
