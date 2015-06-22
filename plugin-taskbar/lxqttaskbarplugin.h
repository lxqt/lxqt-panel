/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012      Razor team
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


#ifndef LXQTTASKBARPLUGIN_H
#define LXQTTASKBARPLUGIN_H

#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"
#include "lxqttaskbar.h"
#include <QDebug>
class LxQtTaskBar;

class LxQtTaskBarPlugin : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtTaskBarPlugin(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtTaskBarPlugin();

    QString themeId() const { return "TaskBar"; }
    virtual Flags flags() const { return HaveConfigDialog | NeedsHandle; }

    QWidget *widget() { return mTaskBar; }
    QDialog *configureDialog();

    void settingsChanged() { mTaskBar->settingsChanged(); }
    void realign();

    bool isSeparate() const { return true; }
    bool isExpandable() const { return true; }
private:
    LxQtTaskBar *mTaskBar;
};

class LxQtTaskBarPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const { return new LxQtTaskBarPlugin(startupInfo);}
};

#endif // LXQTTASKBARPLUGIN_H
