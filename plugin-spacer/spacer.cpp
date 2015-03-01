/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LxQt team
 * Authors:
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

#include "spacer.h"
#include "spacerconfiguration.h"

Spacer::Spacer(const ILxQtPanelPluginStartupInfo &startupInfo) :
    QObject()
    , ILxQtPanelPlugin(startupInfo)
    , mSize(8)
{
    settingsChanged();
}

/************************************************

 ************************************************/
void Spacer::settingsChanged()
{
    mSize = settings()->value("size", 8).toInt();
    setSizes();
}

/************************************************

 ************************************************/
QDialog *Spacer::configureDialog()
{
    return new SpacerConfiguration(settings(), &mSpacer);
}

/************************************************

 ************************************************/
void Spacer::setSizes()
{
    if (panel()->isHorizontal())
    {
        mSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        mSpacer.setFixedWidth(mSize);
        mSpacer.setMinimumHeight(0);
        mSpacer.setMaximumHeight(QWIDGETSIZE_MAX);
    }
    else
    {
        mSpacer.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        mSpacer.setFixedHeight(mSize);
        mSpacer.setMinimumWidth(0);
        mSpacer.setMaximumWidth(QWIDGETSIZE_MAX);
    }
}

/************************************************

 ************************************************/
void Spacer::realign()
{
    setSizes();
}
