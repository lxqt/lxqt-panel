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
#include <XdgIcon>
#include <lxqt-globalkeys.h>
#include <LXQt/Settings>

#include <QFileDialog>

LXQtMainMenuConfiguration::LXQtMainMenuConfiguration(PluginSettings *settings, GlobalKeyShortcut::Action * shortcut, const QString &defaultShortcut, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtMainMenuConfiguration),
    mDefaultShortcut(defaultShortcut),
    mShortcut(shortcut)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("MainMenuConfigurationWindow");
    ui->setupUi(this);

    QIcon folder{XdgIcon::fromTheme("folder")};
    ui->chooseMenuFilePB->setIcon(folder);
    ui->iconPB->setIcon(folder);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    loadSettings();

    connect(ui->showTextCB, SIGNAL(toggled(bool)), this, SLOT(showTextChanged(bool)));
    connect(ui->textLE, SIGNAL(textEdited(QString)), this, SLOT(textButtonChanged(QString)));
    connect(ui->chooseMenuFilePB, SIGNAL(clicked()), this, SLOT(chooseMenuFile()));
    connect(ui->menuFilePathLE, &QLineEdit::textChanged, [&] (QString const & file)
        {
            this->settings().setValue(QLatin1String("menu_file"), file);
        });
    connect(ui->iconCB, &QCheckBox::toggled, [this] (bool value) { this->settings().setValue("ownIcon", value); });
    connect(ui->iconPB, &QAbstractButton::clicked, this, &LXQtMainMenuConfiguration::chooseIcon);
    connect(ui->iconLE, &QLineEdit::textChanged, [&] (QString const & path)
        {
            this->settings().setValue(QLatin1String("icon"), path);
        });

    connect(ui->shortcutEd, SIGNAL(shortcutGrabbed(QString)), this, SLOT(shortcutChanged(QString)));
    connect(ui->shortcutEd->addMenuAction(tr("Reset")), SIGNAL(triggered()), this, SLOT(shortcutReset()));

    connect(ui->customFontCB, SIGNAL(toggled(bool)), this, SLOT(customFontChanged(bool)));
    connect(ui->customFontSizeSB, SIGNAL(valueChanged(int)), this, SLOT(customFontSizeChanged(int)));

    connect(mShortcut, &GlobalKeyShortcut::Action::shortcutChanged, this, &LXQtMainMenuConfiguration::globalShortcutChanged);

    connect(ui->filterMenuCB, &QCheckBox::toggled, [this] (bool enabled)
        {
            this->settings().setValue("filterMenu", enabled);
        });
    connect(ui->filterShowCB, &QCheckBox::toggled, [this] (bool enabled)
        {
            this->settings().setValue("filterShow", enabled);
        });
    connect(ui->filterShowMaxItemsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this] (int value)
        {
            this->settings().setValue("filterShowMaxItems", value);
        });
}

LXQtMainMenuConfiguration::~LXQtMainMenuConfiguration()
{
    delete ui;
}

void LXQtMainMenuConfiguration::loadSettings()
{
    ui->iconCB->setChecked(settings().value("ownIcon", false).toBool());
    ui->iconLE->setText(settings().value("icon", QLatin1String(LXQT_GRAPHICS_DIR"/helix.svg")).toString());
    ui->showTextCB->setChecked(settings().value("showText", false).toBool());
    ui->textLE->setText(settings().value("text", "").toString());

    QString menuFile = settings().value("menu_file", "").toString();
    if (menuFile.isEmpty())
    {
        menuFile = XdgMenu::getMenuFileName();
    }
    ui->menuFilePathLE->setText(menuFile);
    ui->shortcutEd->setText(nullptr != mShortcut ? mShortcut->shortcut() : mDefaultShortcut);

    ui->customFontCB->setChecked(settings().value("customFont", false).toBool());
    LXQt::Settings lxqtSettings("lxqt"); //load system font size as init value
    QFont systemFont;
    lxqtSettings.beginGroup(QLatin1String("Qt"));
    systemFont.fromString(lxqtSettings.value("font", this->font()).toString());
    lxqtSettings.endGroup();
    ui->customFontSizeSB->setValue(settings().value("customFontSize", systemFont.pointSize()).toInt());
    ui->filterMenuCB->setChecked(settings().value("filterMenu", true).toBool());
    const bool filter_show = settings().value("filterShow", true).toBool();
    ui->filterShowCB->setChecked(filter_show);
    ui->filterShowMaxItemsSB->setEnabled(filter_show);
    ui->filterShowMaxItemsSB->setValue(settings().value("filterShowMaxItems", 10).toInt());
}


void LXQtMainMenuConfiguration::textButtonChanged(const QString &value)
{
    settings().setValue("text", value);
}

void LXQtMainMenuConfiguration::showTextChanged(bool value)
{
    settings().setValue("showText", value);
}

void LXQtMainMenuConfiguration::chooseIcon()
{
    QFileInfo f{ui->iconLE->text()};
    QDir dir = f.dir();
    QFileDialog *d = new QFileDialog(this,
                                     tr("Choose icon file"),
                                     !f.filePath().isEmpty() && dir.exists() ? dir.path() : QLatin1String(LXQT_GRAPHICS_DIR),
                                     tr("Images (*.svg *.png)"));
    d->setWindowModality(Qt::WindowModal);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, &QFileDialog::fileSelected, [&] (const QString &icon) {
        ui->iconLE->setText(icon);
    });
    d->show();
}

void LXQtMainMenuConfiguration::chooseMenuFile()
{
    QFileDialog *d = new QFileDialog(this,
                                     tr("Choose menu file"),
                                     QLatin1String("/etc/xdg/menus"),
                                     tr("Menu files (*.menu)"));
    d->setWindowModality(Qt::WindowModal);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, &QFileDialog::fileSelected, [&] (const QString &file) {
        ui->menuFilePathLE->setText(file);
    });
    d->show();
}

void LXQtMainMenuConfiguration::globalShortcutChanged(const QString &/*oldShortcut*/, const QString &newShortcut)
{
    ui->shortcutEd->setText(newShortcut);
}

void LXQtMainMenuConfiguration::shortcutChanged(const QString &value)
{
    if (mShortcut)
        mShortcut->changeShortcut(value);
}

void LXQtMainMenuConfiguration::shortcutReset()
{
    shortcutChanged(mDefaultShortcut);
}

void LXQtMainMenuConfiguration::customFontChanged(bool value)
{
    settings().setValue("customFont", value);
}

void LXQtMainMenuConfiguration::customFontSizeChanged(int value)
{
    settings().setValue("customFontSize", value);
}
