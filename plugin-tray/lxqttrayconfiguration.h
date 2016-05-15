/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Corentin Ferry <cferr@openmailbox.org>
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


#ifndef LXQTTRAYCONFIGURATION_H
#define LXQTTRAYCONFIGURATION_H


#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

#define TRAY_ICON_SIZE_DEFAULT 24

#include <QObject>
#include <QAbstractButton>

namespace Ui {
    class LXQtTrayConfiguration;
}

class LXQtTrayConfiguration : public LXQtPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit LXQtTrayConfiguration(PluginSettings *settings, QWidget *parent = 0);
    ~LXQtTrayConfiguration();

private:
    Ui::LXQtTrayConfiguration *ui;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

private slots:
    /*
      Saves settings in conf file.
    */
    void saveSettings();

};

#endif // LXQTTRAYCONFIGURATION_H
