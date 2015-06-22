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


#ifndef SHOWDESKTOP_H
#define SHOWDESKTOP_H

#include "../panel/ilxqtpanelplugin.h"
#include <QToolButton>


namespace GlobalKeyShortcut
{
class Action;
}

class ShowDesktop :  public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT

public:
    ShowDesktop(const ILxQtPanelPluginStartupInfo &startupInfo);

    virtual QWidget *widget() { return &mButton; }
    virtual QString themeId() const { return "ShowDesktop"; }
private:
    GlobalKeyShortcut::Action * m_key;

private slots:
    void toggleShowingDesktop();
    void shortcutRegistered();

private:
    QToolButton mButton;
};

class ShowDesktopLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new ShowDesktop(startupInfo);
    }
};


#endif

