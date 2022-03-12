/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Daniel Drzisga <sersmicro@gmail.com>
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


#include <QIcon>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <XdgIcon>

#include "directorymenuconfiguration.h"
#include "ui_directorymenuconfiguration.h"


DirectoryMenuConfiguration::DirectoryMenuConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::DirectoryMenuConfiguration),
    mBaseDirectory(QDir::homePath()),
    mDefaultIcon(XdgIcon::fromTheme(QStringLiteral("folder"))),
    mDefaultTerminal(QStringLiteral("/usr/bin/qterminal"))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("DirectoryMenuConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, &QDialogButtonBox::clicked, this, &DirectoryMenuConfiguration::dialogButtonsAction);

    ui->buttonStyleCB->addItem(tr("Only icon"), QLatin1String("Icon"));
    ui->buttonStyleCB->addItem(tr("Only text"), QLatin1String("Text"));
    ui->buttonStyleCB->addItem(tr("Icon and text"), QLatin1String("IconText"));
    connect(ui->buttonStyleCB, QOverload<int>::of(&QComboBox::activated), this, &DirectoryMenuConfiguration::saveSettings);

    loadSettings();
    ui->baseDirectoryB->setIcon(mDefaultIcon);

    connect(ui->baseDirectoryB, &QPushButton::clicked, this, &DirectoryMenuConfiguration::showDirectoryDialog);
    connect(ui->iconB,          &QPushButton::clicked, this, &DirectoryMenuConfiguration::showIconDialog);
    connect(ui->labelB,         &QPushButton::clicked, this, &DirectoryMenuConfiguration::showLabelDialog);
    connect(ui->terminalB,      &QPushButton::clicked, this, &DirectoryMenuConfiguration::showTermDialog);
}

DirectoryMenuConfiguration::~DirectoryMenuConfiguration()
{
    delete ui;
}

void DirectoryMenuConfiguration::loadSettings()
{
    mBaseDirectory.setPath(settings().value(QStringLiteral("baseDirectory"), QDir::homePath()).toString());
    ui->baseDirectoryB->setText(mBaseDirectory.dirName());
    // icon
    bool iconSet = false;
    mIcon = settings().value(QStringLiteral("icon"), QString()).toString();
    if(!mIcon.isNull())
    {
        QIcon buttonIcon = QIcon(mIcon);
        if(!buttonIcon.pixmap(QSize(24,24)).isNull())
        {
            ui->iconB->setIcon(buttonIcon);
            iconSet = true;
        }
    }
    if (!iconSet)
        ui->iconB->setIcon(mDefaultIcon);

    // label
    ui->labelB->setText(settings().value(QStringLiteral("label"), QString()).toString());

    // style
    int index = ui->buttonStyleCB->findData(settings().value(QStringLiteral("buttonStyle"), QLatin1String("Icon")));
    if (index == -1)
        index = 0;
    ui->buttonStyleCB->setCurrentIndex(index);

    ui->terminalB->setText(settings().value(QStringLiteral("defaultTerminal"), QString()).toString());
}

void DirectoryMenuConfiguration::saveSettings()
{
    settings().setValue(QStringLiteral("baseDirectory"), mBaseDirectory.absolutePath());
    settings().setValue(QStringLiteral("icon"), mIcon);
    settings().setValue(QStringLiteral("label"), ui->labelB->text());
    settings().setValue(QStringLiteral("buttonStyle"), ui->buttonStyleCB->itemData(ui->buttonStyleCB->currentIndex()));
    settings().setValue(QStringLiteral("defaultTerminal"), mDefaultTerminal);
}

void DirectoryMenuConfiguration::showDirectoryDialog()
{
    QFileDialog d(this, tr("Choose Base Directory"), mBaseDirectory.absolutePath());
    d.setFileMode(QFileDialog::Directory);
    d.setOptions(QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    d.setWindowModality(Qt::WindowModal);

    if(d.exec() && !d.selectedFiles().isEmpty())
    {
        mBaseDirectory.setPath(d.selectedFiles().constFirst());
        ui->baseDirectoryB->setText(mBaseDirectory.dirName());

        saveSettings();
    }
}

void DirectoryMenuConfiguration::showTermDialog()
{
    QFileDialog d(this, tr("Choose Default Terminal"), QStringLiteral("/usr/bin"));
    d.setFileMode(QFileDialog::ExistingFile);
    d.setWindowModality(Qt::WindowModal);

    if (d.exec() && !d.selectedFiles().isEmpty())
    {
        mDefaultTerminal = d.selectedFiles().constFirst();
        saveSettings();
    }
    ui->terminalB->setText(mDefaultTerminal);
}

void DirectoryMenuConfiguration::showIconDialog()
{
    // prefer the icon theme folder and give priority to the "places" folder
    QString iconDir;
    QString iconThemeName = QIcon::themeName();
    const auto icons = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                                 QStringLiteral("icons"),
                                                 QStandardPaths::LocateDirectory);
    for (const auto& icon : icons)
    {
        QString iconThemeFolder = icon + QLatin1String("/") + iconThemeName;
        if (QDir(iconThemeFolder).exists() && QFileInfo(iconThemeFolder).permission(QFileDevice::ReadUser))
        {
            const QString places = iconThemeFolder + QLatin1String("/places");
            if (QDir(places).exists() && QFileInfo(places).permission(QFileDevice::ReadUser))
                iconDir = places;
            else
                iconDir = iconThemeFolder;
            break;
        }
    }
    QFileDialog d(this, tr("Choose Icon"), iconDir, tr("Icons (*.png *.xpm *.jpg *.svg)"));
    d.setWindowModality(Qt::WindowModal);

    if(d.exec() && !d.selectedFiles().isEmpty())
    {
        QIcon newIcon = QIcon(d.selectedFiles().constFirst());

        if(newIcon.pixmap(QSize(24,24)).isNull())
        {
            QMessageBox::warning(this, tr("Directory Menu"), tr("An error occurred while loading the icon."));
            return;
        }

        ui->iconB->setIcon(newIcon);
        mIcon = d.selectedFiles().constFirst();
        saveSettings();
    }
}

void DirectoryMenuConfiguration::showLabelDialog()
{
    QInputDialog d(this);
    d.setWindowModality(Qt::WindowModal);
    d.setInputMode(QInputDialog::TextInput);
    d.setWindowTitle(tr("Choose Label"));
    d.setLabelText(tr("Label:"));
    QString label = settings().value(QStringLiteral("label"), QString()).toString();
    if (!label.isEmpty())
        d.setTextValue(label);
    if (d.exec())
    {
        ui->labelB->setText(d.textValue());
        saveSettings();
    }
}
