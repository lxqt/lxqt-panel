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

#ifndef LXQTMOUNTPLUGIN_H
#define LXQTMOUNTPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include "../panel/lxqtpanel.h"
#include "button.h"
#include "popup.h"
#include "actions/deviceaction.h"

#include <QIcon>

/*!
\author Petr Vanek <petr@scribus.info>
*/

class LxQtMountPlugin : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT

public:
    LxQtMountPlugin(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtMountPlugin();

    virtual QWidget *widget() { return mButton; }
    virtual QString themeId() const { return QStringLiteral("LxQtMount"); }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

    Popup *popup() { return mPopup; }
    QIcon icon() { return mButton->icon(); };
    QDialog *configureDialog();

public slots:
    void realign();

protected slots:
    virtual void settingsChanged();

private:
    Button *mButton;
    Popup *mPopup;
    DeviceAction *mDeviceAction;
};

class LxQtMountPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)

public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LxQtMountPlugin(startupInfo);
    }
};

#endif
