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


#ifndef LXQTTRAYPLUGIN_H
#define LXQTTRAYPLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include "lxqttrayconfiguration.h"
#include <QObject>

class LXQtTray;
class LXQtTrayPlugin : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    explicit LXQtTrayPlugin(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtTrayPlugin();

    virtual QWidget *widget();
    virtual QString themeId() const { return "Tray"; }
    virtual Flags flags() const { return  PreferRightAlignment | SingleInstance | NeedsHandle | HaveConfigDialog; }
    void realign();

    bool isSeparate() const { return true; }
    QDialog * configureDialog();
    
    void settingsChanged();
    
private:
    LXQtTray *mWidget;
    int mIconSize;

};

class LXQtTrayPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LXQtTrayPlugin(startupInfo);
    }
};

#endif // LXQTTRAYPLUGIN_H
