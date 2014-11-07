/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Jes <zjesclean.gmail@gmail.com>
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

#ifndef _LXQTKBDLAYOUTPLUGIN_H_
#define _LXQTKBDLAYOUTPLUGIN_H_

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtkbdlayoutwidget.h"
#include "lxqtkbdlayout.h"

class LxQtKbdLayoutPlugin: public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtKbdLayoutPlugin(const ILxQtPanelPluginStartupInfo &startupInfo);
    virtual ~LxQtKbdLayoutPlugin();

    virtual QWidget *widget();
    virtual QDialog *configureDialog();
    virtual void realign();

    virtual QString themeId() const
    { return "KbdLayout"; }

    virtual ILxQtPanelPlugin::Flags flags() const
    { return PreferRightAlignment | HaveConfigDialog; }

    virtual bool isSeparate() const
    { return false; }

    virtual void settingsChanged();
private:
    LxQtKbdLayoutWidget  m_content;
    LxQtKbdLayout        m_layout;
};

class LxQtKbdLayoutPluginLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo)
    { return new LxQtKbdLayoutPlugin(startupInfo); }
};

#endif
