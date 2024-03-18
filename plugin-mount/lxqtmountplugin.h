/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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

#ifndef LXQTMOUNTPLUGIN_H
#define LXQTMOUNTPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include "../panel/lxqtpanel.h"
#include "button.h"
#include "popup.h"
#include "actions/deviceaction.h"
#include "actions/ejectaction.h"

#include <QIcon>

namespace GlobalKeyShortcut
{
    class Action;
}

/*!
\author Petr Vanek <petr@scribus.info>
*/

class LXQtMountPlugin : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT

public:
    LXQtMountPlugin(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtMountPlugin() override;

    QWidget *widget() override { return mButton; }
    QString themeId() const override { return QLatin1String("LXQtMount"); }
    ILXQtPanelPlugin::Flags flags() const override { return PreferRightAlignment | HaveConfigDialog; }

    Popup *popup() override { return mPopup; }
    QIcon icon() override { return mButton->icon(); }
    QDialog *configureDialog() override;

public slots:
    void realign() override;

protected slots:
    void settingsChanged() override;
    void shortcutRegistered();

private:
    Button *mButton;
    Popup *mPopup;
    DeviceAction *mDeviceAction;
    EjectAction *mEjectAction;
    GlobalKeyShortcut::Action *mKeyEject;
};

class LXQtMountPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)

public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LXQtMountPlugin(startupInfo);
    }
};

#endif
