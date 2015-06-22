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


#ifndef PANELSCREENSAVER_H
#define PANELSCREENSAVER_H

#include "../panel/ilxqtpanelplugin.h"
#include <QToolButton>

namespace LxQt {
class ScreenSaver;
}
namespace GlobalKeyShortcut
{
class Action;
}

class PanelScreenSaver : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT

public:
    PanelScreenSaver(const ILxQtPanelPluginStartupInfo &startupInfo);

    virtual QWidget *widget() { return &mButton; }
    virtual QString themeId() const { return "PanelScreenSaver"; }

private slots:
    void shortcutRegistered();

private:
    QToolButton  mButton;
    LxQt::ScreenSaver * mSaver;
    GlobalKeyShortcut::Action * mShortcutKey;
};

class PanelScreenSaverLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new PanelScreenSaver(startupInfo);
    }
};

#endif

