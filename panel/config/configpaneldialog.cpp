/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

ConfigPanelDialog::ConfigPanelDialog(LxQtPanel *panel, QWidget *parent):
    LxQt::ConfigDialog(tr("Configure Panel"), panel->settings(), parent),
    mPanelPage(nullptr),
    mPluginsPage(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mPanelPage = new ConfigPanelWidget(panel, this);
    addPage(mPanelPage, tr("Configure Panel"), QStringLiteral("configure"));
    connect(this, &ConfigPanelDialog::reset, mPanelPage, &ConfigPanelWidget::reset);
    connect(this, &ConfigPanelDialog::accepted, [panel] {
         panel->saveSettings();
    });

    mPluginsPage = new ConfigPluginsWidget(panel, this);
    addPage(mPluginsPage, tr("Manage Widgets"), QStringLiteral("preferences-plugin"));
    connect(this, &ConfigPanelDialog::reset, mPluginsPage, &ConfigPluginsWidget::reset);
    connect(this, &ConfigPanelDialog::accepted, [panel] {
        panel->saveSettings();
   });
}

void ConfigPanelDialog::showConfigPanelPage()
{
    showPage(mPanelPage);
}

void ConfigPanelDialog::showConfigPluginsPage()
{
    showPage(mPluginsPage);
}
