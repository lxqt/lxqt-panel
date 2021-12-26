/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
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

#include "configpaneldialog.h"

ConfigPanelDialog::ConfigPanelDialog(LXQtPanel *panel, QWidget *parent):
    LXQt::ConfigDialog(tr("Configure Panel"), panel->settings(), parent),
    mPlacementPage(nullptr),
    mStylingPage(nullptr),
    mPluginsPage(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mPlacementPage = new ConfigPlacement(panel, this);
    addPage(mPlacementPage, tr("Placement"), QLatin1String("configure-toolbars"));
    connect(this, &ConfigPanelDialog::reset, mPlacementPage, &ConfigPlacement::reset);

    mStylingPage = new ConfigStyling(panel, this);
    addPage(mStylingPage, tr("Styling"), QLatin1String("colormanagement"));
    connect(this, &ConfigPanelDialog::reset, mStylingPage, &ConfigStyling::reset);

    mPluginsPage = new ConfigPluginsWidget(panel, this);
    addPage(mPluginsPage, tr("Widgets"), QLatin1String("preferences-plugin"));
    connect(this, &ConfigPanelDialog::reset, mPluginsPage, &ConfigPluginsWidget::reset);

    connect(this, &ConfigPanelDialog::accepted, panel, [panel] {
        panel->saveSettings();
    });
}

void ConfigPanelDialog::showConfigPlacementPage()
{
    showPage(mPlacementPage);
}

void ConfigPanelDialog::showConfigStylingPage()
{
    showPage(mStylingPage);
}

void ConfigPanelDialog::showConfigPluginsPage()
{
    showPage(mPluginsPage);
}

void ConfigPanelDialog::updateIconThemeSettings()
{
    mStylingPage->updateIconThemeSettings();
}
