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


#ifndef LXQTCPULOADPLUGIN_H
#define LXQTCPULOADPLUGIN_H


#include "../panel/ilxqtpanelplugin.h"
#include <QObject>
class LxQtCpuLoad;

class LxQtCpuLoadPlugin: public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    explicit LxQtCpuLoadPlugin(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtCpuLoadPlugin();

    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual QWidget *widget();
    virtual QString themeId() const { return "CpuLoad"; }

    bool isSeparate() const { return true; }
    QDialog *configureDialog();

protected:
    virtual void settingsChanged();

private:
    QWidget *mWidget;
    LxQtCpuLoad *mContent;
};


class LxQtCpuLoadPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LxQtCpuLoadPlugin(startupInfo);
    }
};


#endif // LXQTCPULOADPLUGIN_H
