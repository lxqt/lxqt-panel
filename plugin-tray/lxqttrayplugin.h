/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2.1+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2013 Razor team
 *            2022 LXQt team
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

#pragma once

#include "../panel/ilxqtpanelplugin.h"

#include <QObject>

#include <QDebug>

class FdoSelectionManager;
class LXQtTrayPlugin : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    explicit LXQtTrayPlugin(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtTrayPlugin() override;

    QWidget *widget() override;
    QString themeId() const override { return QStringLiteral("Tray"); }
    Flags flags() const override { return PreferRightAlignment | SingleInstance | NeedsHandle; }

    bool isSeparate() const override { return true; }

private:
    std::unique_ptr<FdoSelectionManager> mManager;

};

class LXQtTrayPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const;
};
