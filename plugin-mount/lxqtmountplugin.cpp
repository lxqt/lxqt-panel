/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2010-2011 Razor team
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
#include "configuration.h"

#include <Solid/DeviceNotifier>

LxQtMountPlugin::LxQtMountPlugin(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mPopup(nullptr),
    mDeviceAction(nullptr)
{
    mButton = new Button;
    mPopup = new Popup(this);

    connect(mButton, &QToolButton::clicked, mPopup, &Popup::showHide);
    connect(mPopup, &Popup::visibilityChanged, mButton, &QToolButton::setDown);
}

LxQtMountPlugin::~LxQtMountPlugin()
{
    delete mButton;
    delete mPopup;
}

QDialog *LxQtMountPlugin::configureDialog()
{
    if (mPopup)
        mPopup->hide();

    Configuration *configWindow = new Configuration(*settings());
    configWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    return configWindow;
}

void LxQtMountPlugin::realign()
{
    //nothing to do
}

void LxQtMountPlugin::settingsChanged()
{
    QString s = settings()->value(QStringLiteral(CFG_KEY_ACTION)).toString();
    DeviceAction::ActionId actionId = DeviceAction::stringToActionId(s, DeviceAction::ActionMenu);

    if (mDeviceAction == nullptr || mDeviceAction->Type() != actionId)
    {
        delete mDeviceAction;
        mDeviceAction = DeviceAction::create(actionId, this);

        connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded,
                mDeviceAction, &DeviceAction::onDeviceAdded);

        connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved,
                mDeviceAction, &DeviceAction::onDeviceRemoved);
    }

}
