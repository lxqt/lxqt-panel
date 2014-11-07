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

#ifndef _LXQTKBDLAYOUT_H_
#define _LXQTKBDLAYOUT_H_

#include <QObject>
#include <QSharedDataPointer>

class LxQtKbdLayoutPrivate;
class QSettings;

enum class SwitchMode
{
    Global,
    Application,
    Window
};

Q_DECLARE_METATYPE(SwitchMode)

class LxQtKbdLayout: public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LxQtKbdLayout)
public:
    LxQtKbdLayout(QSettings *set);
    virtual ~LxQtKbdLayout();
public slots:
    void reset();
signals:
    void changed(const QString &lay);
    void capsChanged(bool);
private:
    QScopedPointer<LxQtKbdLayoutPrivate> d_ptr;
};

#endif
