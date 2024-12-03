/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "ilxqtabstractwmiface.h"

ILXQtAbstractWMInterface::ILXQtAbstractWMInterface(QObject *parent)
    : QObject(parent)
{

}

void ILXQtAbstractWMInterface::moveApplicationToPrevNextDesktop(WId windowId, bool next)
{
    int count = getWorkspacesCount();
    if (count <= 1)
        return;

    int targetWorkspace = getWindowWorkspace(windowId) + (next ? 1 : -1);

    // Wrap around
    if (targetWorkspace > count)
        targetWorkspace = 1; //Ids are 1-based
    else if (targetWorkspace < 1)
        targetWorkspace = count;

    setWindowOnWorkspace(windowId, targetWorkspace);
}

int ILXQtAbstractWMInterface::onAllWorkspacesEnum() const
{
    // Virtual destops have 1-based indexes.
    // NOTE: The real value of this enum may be negative (as in X11).
    return 0;
}
