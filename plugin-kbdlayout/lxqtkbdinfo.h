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

#ifndef _LXQTKBDINFO_H_
#define _LXQTKBDINFO_H_

#include <QList>
#include <QString>
typedef struct _XDisplay Display;
typedef	struct _XkbDesc XkbDescRec;
typedef unsigned long Atom;

class LxQtKbdInfo
{
public:
    LxQtKbdInfo();
public:
    bool readKeyboardInfo();

    const QString & currentSym() const
    { return m_keyboardInfo[m_current].sym; }

    int currentGroup() const
    { return m_current; }

    void setCurrentGroup(int group)
    { m_current = group; }

    const QString & currentName() const
    { return m_keyboardInfo[m_current].name; }
private:
    void resetInfoToDefault();
    QString atomName(Display *display, Atom name);
    bool readState(XkbDescRec *kbd);
    bool parseKbdLayout(const QString & layout);
private:
    struct Info
    {
        QString sym;
        QString name;
    };
    QList<Info> m_keyboardInfo = {{"us", ""}};
    int         m_current      = 0;
};

#endif
