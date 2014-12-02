/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include "lxqtmainmenuconfiguration.h"
#include "ui_lxqtmainmenuconfiguration.h"
#include <XdgMenu>

#include <QFileDialog>

LxQtMainMenuConfiguration::LxQtMainMenuConfiguration(QSettings &settings, const QString &defaultShortcut, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LxQtMainMenuConfiguration),
    mSettings(settings),
    mOldSettings(settings),
    mDefaultShortcut(defaultShortcut)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("MainMenuConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();

    connect(ui->showTextCB, SIGNAL(toggled(bool)), this, SLOT(showTextChanged(bool)));
    connect(ui->textLE, SIGNAL(textEdited(QString)), this, SLOT(textButtonChanged(QString)));
    connect(ui->chooseMenuFilePB, SIGNAL(clicked()), this, SLOT(chooseMenuFile()));
    
    connect(ui->shortcutEd, SIGNAL(shortcutGrabbed(QString)), this, SLOT(shortcutChanged(QString)));
    connect(ui->shortcutEd->addMenuAction(tr("Reset")), SIGNAL(triggered()), this, SLOT(shortcutReset()));

    connect(ui->customFontCB, SIGNAL(toggled(bool)), this, SLOT(customFontChanged(bool)));
    connect(ui->customFontSizeSB, SIGNAL(valueChanged(int)), this, SLOT(customFontSizeChanged(int)));
}

LxQtMainMenuConfiguration::~LxQtMainMenuConfiguration()
{
    delete ui;
}

void LxQtMainMenuConfiguration::loadSettings()
{
    ui->showTextCB->setChecked(mSettings.value("showText", false).toBool());
    ui->textLE->setText(mSettings.value("text", "").toString());

    QString menuFile = mSettings.value("menu_file", "").toString();
    if (menuFile.isEmpty())
    {
        menuFile = XdgMenu::getMenuFileName();
    }
    ui->menuFilePathLE->setText(menuFile);
    ui->shortcutEd->setText(mSettings.value("shortcut", "Alt+F1").toString());

    ui->customFontCB->setChecked(mSettings.value("customFont", false).toBool());
    LxQt::Settings lxqtSettings("lxqt"); //load system font size as init value
    QFont systemFont;
    lxqtSettings.beginGroup(QLatin1String("Qt"));
    systemFont.fromString(lxqtSettings.value("font", this->font()).toString());
    lxqtSettings.endGroup();
    ui->customFontSizeSB->setValue(mSettings.value("customFontSize", systemFont.pointSize()).toInt());
}

void LxQtMainMenuConfiguration::textButtonChanged(const QString &value)
{
    mSettings.setValue("text", value);
}

void LxQtMainMenuConfiguration::showTextChanged(bool value)
{
    mSettings.setValue("showText", value);
}

void LxQtMainMenuConfiguration::chooseMenuFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose menu file"), "~", tr("Menu files (*.menu)"));
    if (!path.isEmpty())
    {
        ui->menuFilePathLE->setText(path);
        mSettings.setValue("menu_file", path);
    }
}

void LxQtMainMenuConfiguration::shortcutChanged(const QString &value)
{
    ui->shortcutEd->setText(value);
    mSettings.setValue("shortcut", value);
}

void LxQtMainMenuConfiguration::shortcutReset()
{
    shortcutChanged(mDefaultShortcut);
}

void LxQtMainMenuConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}

void LxQtMainMenuConfiguration::customFontChanged(bool value)
{
    mSettings.setValue("customFont", value);
}

void LxQtMainMenuConfiguration::customFontSizeChanged(int value)
{
    mSettings.setValue("customFontSize", value);
}
