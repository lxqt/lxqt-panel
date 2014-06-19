/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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


#include "lxqtmountplugin.h"
#include "lxqtmountconfiguration.h"
#include <lxqtmount/lxqtmount.h>
#include "actions/deviceaction.h"
#include "popup.h"
#include "mountbutton.h"

#include <QtDebug>

Q_EXPORT_PLUGIN2(mount, LxQtMountPluginLibrary)


LxQtMountPlugin::LxQtMountPlugin(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
	mPopup(NULL),
    mMountManager(NULL),
    mDeviceAction(0)
{
    mButton = new MountButton();
    connect(mButton, SIGNAL(clicked(bool)), SLOT(buttonClicked()));
}


LxQtMountPlugin::~LxQtMountPlugin()
{
    delete mButton;
    delete mDeviceAction;
}


QWidget *LxQtMountPlugin::widget()
{
    return mButton;
}


QDialog *LxQtMountPlugin::configureDialog()
{
    if(mPopup)
        mPopup->hide();
    LxQtMountConfiguration *configWindow = new LxQtMountConfiguration(*settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    return configWindow;
}


QIcon LxQtMountPlugin::icon() const
{
    return mButton->icon();
}


void LxQtMountPlugin::settingsChanged()
{
    QString s = settings()->value("newDeviceAction").toString();
    DeviceAction::ActionId actionId = DeviceAction::stringToActionId(s, DeviceAction::ActionMenu);

    delete mDeviceAction;
    mDeviceAction = DeviceAction::create(actionId, this);

    if(mMountManager)
    {
        connect(mMountManager, SIGNAL(deviceAdded(LxQt::MountDevice*)),
                mDeviceAction, SLOT(deviceAdded(LxQt::MountDevice*)));

        connect(mMountManager, SIGNAL(deviceRemoved(LxQt::MountDevice*)),
                mDeviceAction, SLOT(deviceRemoved(LxQt::MountDevice*)));
    }
}

void LxQtMountPlugin::buttonClicked()
{
    if(!mMountManager)
    {
        mMountManager = new LxQt::MountManager(this);
        mPopup = new Popup(mMountManager, this, mButton);
        settingsChanged();

        connect(mPopup, SIGNAL(visibilityChanged(bool)), mButton, SLOT(setDown(bool)));
        mMountManager->update();
    }
    mPopup->showHide();
}
