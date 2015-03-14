/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef LXQTNETWORKMONITORPLUGIN_H
#define LXQTNETWORKMONITORPLUGIN_H


#include "../panel/ilxqtpanelplugin.h"
#include <QObject>
class LxQtNetworkMonitor;

class LxQtNetworkMonitorPlugin: public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    explicit LxQtNetworkMonitorPlugin(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtNetworkMonitorPlugin();

    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual QWidget *widget();
    virtual QString themeId() const { return "NetworkMonitor"; }

    bool isSeparate() const { return false; }
    QDialog *configureDialog();

protected:
    virtual void settingsChanged();

private:
    LxQtNetworkMonitor *mWidget;
};


class LxQtNetworkMonitorPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LxQtNetworkMonitorPlugin(startupInfo);
    }
};


#endif // LXQTNETWORKMONITORPLUGIN_H
