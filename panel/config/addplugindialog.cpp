/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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

#include "ui_addplugindialog.h"
#include "addplugindialog.h"
#include "plugin.h"
#include "ilxqtpanelplugin.h"

#include <LXQt/HtmlDelegate>
#include <XdgIcon>
#include <XdgDirs>

#include <QString>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QIcon>

#define SEARCH_ROLE  Qt::UserRole
#define INDEX_ROLE   SEARCH_ROLE+1

AddPluginDialog::AddPluginDialog(PanelPluginsModel *model, QWidget *parent):
    QDialog(parent),
    ui(new Ui::AddPluginDialog),
    mModel(model)
{
    ui->setupUi(this);

    QStringList desktopFilesDirs;
    desktopFilesDirs << QString(getenv("LXQT_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    desktopFilesDirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/lxqt/lxqt-panel");
    desktopFilesDirs << PLUGIN_DESKTOPS_DIR;

    mPlugins = LxQt::PluginInfo::search(desktopFilesDirs, QStringLiteral("LxQtPanel/Plugin"), QStringLiteral("*"));
    std::sort(mPlugins.begin(), mPlugins.end(), [](const LxQt::PluginInfo &p1, const LxQt::PluginInfo &p2) {
        return p1.name() < p2.name() || (p1.name() == p2.name() && p1.comment() < p2.comment());
    });

    ui->pluginList->setItemDelegate(new LxQt::HtmlDelegate(QSize(32, 32), ui->pluginList));
    ui->pluginList->setContextMenuPolicy(Qt::CustomContextMenu);

    filter();

    // search
    mSearchTimer.setInterval(300);
    mSearchTimer.setSingleShot(true);
    connect(ui->searchEdit, &QLineEdit::textEdited,
            &mSearchTimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(&mSearchTimer, &QTimer::timeout, this, &AddPluginDialog::filter);
    connect(ui->pluginList, &QListWidget::doubleClicked, this, &AddPluginDialog::emitPluginSelected);
    connect(ui->addButton, &QPushButton::clicked, this, &AddPluginDialog::emitPluginSelected);
}

AddPluginDialog::~AddPluginDialog()
{
    delete ui;
}

void AddPluginDialog::filter()
{
    QListWidget* pluginList = ui->pluginList;

    const int curr_item = 0 < pluginList->count() ? pluginList->currentRow() : 0;
    pluginList->clear();

    static QIcon fallIco = XdgIcon::fromTheme("preferences-plugin");

    int pluginCount = mPlugins.length();
    for (int i = 0; i < pluginCount; ++i)
    {
        const LxQt::PluginInfo &plugin = mPlugins.at(i);

        QString s = QString("%1 %2 %3 %4").arg(plugin.name(),
                                               plugin.comment(),
                                               plugin.value("Name").toString(),
                                               plugin.value("Comment").toString());
        if (!s.contains(ui->searchEdit->text(), Qt::CaseInsensitive))
            continue;

        QListWidgetItem* item = new QListWidgetItem(ui->pluginList);
        item->setText(QString("<b>%1</b><br>\n%2\n").arg(plugin.name(), plugin.comment()));
        item->setIcon(plugin.icon(fallIco));
        item->setData(INDEX_ROLE, i);

        // disable single-instances plugins already in use
        Plugin *p = mModel->pluginByID(plugin.id());
        if (p && p->iPlugin()->flags().testFlag(ILxQtPanelPlugin::SingleInstance))
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            item->setBackground(palette().brush(QPalette::Disabled, QPalette::Text));
        }
    }

    if (pluginCount > 0)
        ui->pluginList->setCurrentRow(curr_item < pluginCount ? curr_item : pluginCount - 1);
}

void AddPluginDialog::emitPluginSelected()
{
    QListWidget* pluginList = ui->pluginList;
    if (pluginList->currentItem() && pluginList->currentItem()->isSelected())
    {
        LxQt::PluginInfo plugin = mPlugins.at(pluginList->currentItem()->data(INDEX_ROLE).toInt());
        emit pluginSelected(plugin);
    }
}

