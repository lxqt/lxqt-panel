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
#include <QWindow>
#include <QScreen>
#include <QApplication>

ConfigPanelDialog::ConfigPanelDialog(LXQtPanel *panel, QWidget *parent):
    LXQt::ConfigDialog(tr("Configure Panel"), panel->settings(), parent),
    mPanelPage(nullptr),
    mPluginsPage(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);

    mPanelPage = new ConfigPanelWidget(panel, this);
    addPage(mPanelPage, tr("Panel"), QLatin1String("configure"));
    connect(this, &ConfigPanelDialog::reset, mPanelPage, &ConfigPanelWidget::reset);

    mPluginsPage = new ConfigPluginsWidget(panel, this);
    addPage(mPluginsPage, tr("Widgets"), QLatin1String("preferences-plugin"));
    connect(this, &ConfigPanelDialog::reset, mPluginsPage, &ConfigPluginsWidget::reset);

    connect(this, &ConfigPanelDialog::accepted, panel, [panel] {
        panel->saveSettings();
    });
}

void ConfigPanelDialog::show()
{
    LXQt::ConfigDialog::show();

    QTimer::singleShot(0, this, [this] {
        // fit into available desktop geometry
        QScreen *sc = nullptr;
        if (auto p = qobject_cast<QWidget*>(parent()))
        {
            if (QWindow *win = p->windowHandle())
                sc = win->screen();
        }
        if (sc == nullptr)
            sc = QApplication::primaryScreen();
        if (sc != nullptr)
        {
            QSize ag = sc->availableVirtualGeometry().size() - QSize(100, 100);
            if (!ag.isEmpty())
            {
                // also, avoid scrollbars on the first page if possible
                QSize diff = mPanelPage->extraLayoutSize();
                resize((size() + diff).boundedTo(ag));
            }
        }
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

void ConfigPanelDialog::updateIconThemeSettings()
{
    mPanelPage->updateIconThemeSettings();
}
