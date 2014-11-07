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
#include <QStringList>
#include <QX11Info>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "lxqtkbdinfo.h"

LxQtKbdInfo::LxQtKbdInfo()
{
}

bool LxQtKbdInfo::parseKbdLayout(const QString & layout)
{
    QStringList its = layout.split("+");
    if (its.length() < m_keyboardInfo.size() + 1)
        return false;

    QRegExp re("([^:\\(]+)(\\(.*\\))?(:(\\d))?", Qt::CaseSensitive, QRegExp::RegExp2);

    for(int i = 1; i < m_keyboardInfo.size()+1; ++i){
        if (re.indexIn(its[i]) != -1){
            int gid = re.cap(4).toInt();
            if (gid) gid--;
            m_keyboardInfo[gid].sym = re.cap(1);
        }
    }
    return true;
}

bool LxQtKbdInfo::readState(XkbDescRec *kbd)
{
    kbd->dpy = QX11Info::display();
    if (XkbGetControls(kbd->dpy, XkbAllControlsMask, kbd) == Success) {
        uint ngroups = kbd->ctrls->num_groups;
        if (ngroups >= 1){
            XkbStateRec state;
            if (XkbGetState(kbd->dpy, XkbUseCoreKbd, &state) == Success) {
                m_current = state.group;

                if (XkbGetNames(kbd->dpy, XkbSymbolsNameMask, kbd) != Success){
                    qWarning() << "can't get Xkb symbol description";
                    return false;
                }

                if (XkbGetNames(kbd->dpy, XkbGroupNamesMask, kbd) != Success)
                    qWarning() << "Failed to get keyboard description";

                return true;
            } else {
                qWarning() << "can't get Xkb state";
                return false;
            }
        }
    }
    return false;
}

void LxQtKbdInfo::resetInfoToDefault()
{
    m_current = 0;
    m_keyboardInfo.append({"us", ""});
}

bool LxQtKbdInfo::readKeyboardInfo()
{
    m_keyboardInfo.clear();

    XkbDescRec * kbd = XkbAllocKeyboard();
    if (!kbd){
        resetInfoToDefault();
        qWarning() << "can't alloc kbd info";
        return false;
    }

    if (readState(kbd)){
        Atom symNameAtom = kbd->names->symbols;
        if (symNameAtom) {
            for (int i = 0; i < 4; i++) {
                if (kbd->names == NULL || kbd->names->groups[i] == 0) {
                    continue;
                }
                QString lab = atomName(kbd->dpy, kbd->names->groups[i]);
                m_keyboardInfo.append({"", lab});
            }
            QString sym = atomName(kbd->dpy, symNameAtom);
            if (!sym.isEmpty()){
                XkbFreeKeyboard(kbd, 0, True);
                return parseKbdLayout(sym);
            }
        }
    } else {
        qWarning() << "can't get Xkb controls";
    }

    XkbFreeKeyboard(kbd, 0, True);

    return false;
}

QString LxQtKbdInfo::atomName(Display *display, Atom name)
{
    char * retName = XGetAtomName(display, name);
    QString ret = retName;
    XFree(retName);
    return ret;
}

