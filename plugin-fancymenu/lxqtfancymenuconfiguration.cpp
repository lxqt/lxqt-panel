/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */


#include "lxqtfancymenuconfiguration.h"
#include "ui_lxqtfancymenuconfiguration.h"
#include <XdgMenu>
#include <XdgIcon>
#include <lxqt-globalkeys.h>
#include <LXQt/Settings>

#include <QAction>
#include <QFileDialog>

#include "lxqtfancymenutypes.h"

LXQtFancyMenuConfiguration::LXQtFancyMenuConfiguration(PluginSettings *settings, GlobalKeyShortcut::Action * shortcut, const QString &defaultShortcut, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtFancyMenuConfiguration),
    mDefaultShortcut(defaultShortcut),
    mShortcut(shortcut),
    mLockSettingChanges(false)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QStringLiteral("FancyMenuConfigurationWindow"));
    ui->setupUi(this);

    fillButtonPositionComboBox();
    fillCategoryPositionComboBox();

    QIcon folder{XdgIcon::fromTheme(QStringLiteral("folder"))};
    ui->chooseMenuFilePB->setIcon(folder);
    ui->iconPB->setIcon(folder);

    connect(ui->buttons, &QDialogButtonBox::clicked, this, &LXQtFancyMenuConfiguration::dialogButtonsAction);

    loadSettings();

    connect(ui->showTextCB,       &QAbstractButton::toggled, this, &LXQtFancyMenuConfiguration::showTextChanged);
    connect(ui->textLE,           &QLineEdit::textEdited,    this, &LXQtFancyMenuConfiguration::textButtonChanged);
    connect(ui->chooseMenuFilePB, &QAbstractButton::clicked, this, &LXQtFancyMenuConfiguration::chooseMenuFile);
    connect(ui->menuFilePathLE,   &QLineEdit::textChanged,   this, [&] (QString const & file) {
        if (!mLockSettingChanges)
            this->settings().setValue(QLatin1String("menu_file"), file);
    });
    connect(ui->iconCB, &QCheckBox::toggled, this, [this] (bool value) {
        if (!mLockSettingChanges)
            this->settings().setValue(QStringLiteral("ownIcon"), value);
    });
    connect(ui->iconPB, &QAbstractButton::clicked, this, &LXQtFancyMenuConfiguration::chooseIcon);
    connect(ui->iconLE, &QLineEdit::textChanged, this, [&] (QString const & path) {
        if (!mLockSettingChanges)
            this->settings().setValue(QLatin1String("icon"), path);
    });

    connect(ui->shortcutEd, &ShortcutSelector::shortcutGrabbed, this, &LXQtFancyMenuConfiguration::shortcutChanged);
    connect(ui->shortcutEd->addMenuAction(tr("Reset")), &QAction::triggered, this, &LXQtFancyMenuConfiguration::shortcutReset);

    connect(ui->customFontCB, &QAbstractButton::toggled, this, &LXQtFancyMenuConfiguration::customFontChanged);
    connect(ui->customFontSizeSB, &QSpinBox::valueChanged, this, &LXQtFancyMenuConfiguration::customFontSizeChanged);

    connect(ui->autoSelCB, &QAbstractButton::toggled, this, [this] (bool checked) {
        this->settings().setValue(QStringLiteral("autoSel"), checked);
    });
    connect(ui->autoSelSB, &QSpinBox::valueChanged, this, [this] (int value) {
        this->settings().setValue(QStringLiteral("autoSelDelay"), value);
    });

    connect(mShortcut, &GlobalKeyShortcut::Action::shortcutChanged, this, &LXQtFancyMenuConfiguration::globalShortcutChanged);

    connect(ui->filterClearCB, &QCheckBox::toggled, this, [this] (bool value) {
        if (!mLockSettingChanges)
            this->settings().setValue(QStringLiteral("filterClear"), value);
    });

    connect(ui->buttRowPosCB, &QComboBox::activated, this, &LXQtFancyMenuConfiguration::buttonRowPositionChanged);
    connect(ui->categoryViewPosCB, &QComboBox::activated, this, &LXQtFancyMenuConfiguration::categoryPositionChanged);
}

LXQtFancyMenuConfiguration::~LXQtFancyMenuConfiguration()
{
    delete ui;
}

void LXQtFancyMenuConfiguration::fillButtonPositionComboBox()
{
    ui->buttRowPosCB->addItem(tr("Bottom"), LXQtFancyMenuButtonPosition::Bottom);
    ui->buttRowPosCB->addItem(tr("Top"), LXQtFancyMenuButtonPosition::Top);
}

void LXQtFancyMenuConfiguration::fillCategoryPositionComboBox()
{
    ui->categoryViewPosCB->addItem(tr("Left"), LXQtFancyMenuCategoryPosition::Left);
    ui->categoryViewPosCB->addItem(tr("Right"), LXQtFancyMenuCategoryPosition::Right);
}

void LXQtFancyMenuConfiguration::loadSettings()
{
    mLockSettingChanges = true;

    ui->iconCB->setChecked(settings().value(QStringLiteral("ownIcon"), false).toBool());
    ui->iconLE->setText(settings().value(QStringLiteral("icon"), QLatin1String(LXQT_GRAPHICS_DIR"/helix.svg")).toString());
    ui->showTextCB->setChecked(settings().value(QStringLiteral("showText"), false).toBool());
    ui->textLE->setText(settings().value(QStringLiteral("text"), QString()).toString());

    QString menuFile = settings().value(QStringLiteral("menu_file"), QString()).toString();
    if (menuFile.isEmpty())
        menuFile = XdgMenu::getMenuFileName(QLatin1String("panel-applications.menu"));
    else if (!menuFile.contains(QLatin1String("/")))
        menuFile = XdgMenu::getMenuFileName(menuFile);
    ui->menuFilePathLE->setText(menuFile);

    if (QGuiApplication::platformName() != QStringLiteral("xcb")) {
        ui->shortcutGB->setEnabled(false);
        ui->shortcutGB->setToolTip(QStringLiteral("Modify or add a shortcut to 'lxqt-qdbus openmenu'\nin the compositor settings under Wayland"));
        ui->shortcutEd->setText(QStringLiteral("Super_L"));
    }
    else {
    ui->shortcutEd->setText(nullptr != mShortcut ? mShortcut->shortcut() : mDefaultShortcut);
    }
    ui->customFontCB->setChecked(settings().value(QStringLiteral("customFont"), false).toBool());
    LXQt::Settings lxqtSettings(QStringLiteral("lxqt")); //load system font size as init value
    QFont systemFont;
    lxqtSettings.beginGroup(QLatin1String("Qt"));
    systemFont.fromString(lxqtSettings.value(QStringLiteral("font"), this->font()).toString());
    lxqtSettings.endGroup();
    ui->customFontSizeSB->setValue(settings().value(QStringLiteral("customFontSize"), systemFont.pointSize()).toInt());
    ui->filterClearCB->setChecked(settings().value(QStringLiteral("filterClear"), true).toBool());

    ui->autoSelSB->setValue(settings().value(QStringLiteral("autoSelDelay"), 250).toInt());
    ui->autoSelCB->setChecked(settings().value(QStringLiteral("autoSel"), false).toBool());

    bool buttonsAtTop = settings().value(QStringLiteral("buttonsAtTop"), false).toBool();
    int buttRowPosIdx = ui->buttRowPosCB->findData(buttonsAtTop ? LXQtFancyMenuButtonPosition::Top : LXQtFancyMenuButtonPosition::Bottom);
    ui->buttRowPosCB->setCurrentIndex(buttRowPosIdx);

    bool categoriesAtRight = settings().value(QStringLiteral("categoriesAtRight"), true).toBool();
    int categoryPosIdx = ui->categoryViewPosCB->findData(categoriesAtRight ? LXQtFancyMenuCategoryPosition::Right : LXQtFancyMenuCategoryPosition::Left);
    ui->categoryViewPosCB->setCurrentIndex(categoryPosIdx);

    mLockSettingChanges = false;
}


void LXQtFancyMenuConfiguration::textButtonChanged(const QString &value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("text"), value);
}

void LXQtFancyMenuConfiguration::showTextChanged(bool value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("showText"), value);
}

void LXQtFancyMenuConfiguration::chooseIcon()
{
    QFileInfo f{ui->iconLE->text()};
    QDir dir = f.dir();
    QFileDialog *d = new QFileDialog(this,
                                     tr("Choose icon file"),
                                     !f.filePath().isEmpty() && dir.exists() ? dir.path() : QLatin1String(LXQT_GRAPHICS_DIR),
                                     tr("Images (*.svg *.png)"));
    d->setWindowModality(Qt::WindowModal);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, &QFileDialog::fileSelected, this, [&] (const QString &icon) {
        ui->iconLE->setText(icon);
    });
    d->show();
}

void LXQtFancyMenuConfiguration::chooseMenuFile()
{
    QFileDialog *d = new QFileDialog(this,
                                     tr("Choose menu file"),
                                     QLatin1String("/etc/xdg/menus"),
                                     tr("Menu files (*.menu)"));
    d->setWindowModality(Qt::WindowModal);
    d->setAttribute(Qt::WA_DeleteOnClose);
    connect(d, &QFileDialog::fileSelected, this, [&] (const QString &file) {
        ui->menuFilePathLE->setText(file);
    });
    d->show();
}

void LXQtFancyMenuConfiguration::globalShortcutChanged(const QString &/*oldShortcut*/, const QString &newShortcut)
{
    ui->shortcutEd->setText(newShortcut);
}

void LXQtFancyMenuConfiguration::shortcutChanged(const QString &value)
{
    if (mShortcut)
        mShortcut->changeShortcut(value);
}

void LXQtFancyMenuConfiguration::shortcutReset()
{
    shortcutChanged(mDefaultShortcut);
}

void LXQtFancyMenuConfiguration::customFontChanged(bool value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("customFont"), value);
}

void LXQtFancyMenuConfiguration::customFontSizeChanged(int value)
{
    if (!mLockSettingChanges)
        settings().setValue(QStringLiteral("customFontSize"), value);
}

void LXQtFancyMenuConfiguration::buttonRowPositionChanged(int idx)
{
    if (mLockSettingChanges)
        return;
    LXQtFancyMenuButtonPosition pos = LXQtFancyMenuButtonPosition(this->ui->buttRowPosCB->itemData(idx).toInt());
    bool value = (pos == LXQtFancyMenuButtonPosition::Top);
    this->settings().setValue(QStringLiteral("buttonsAtTop"), value);
}

void LXQtFancyMenuConfiguration::categoryPositionChanged(int idx)
{
    if (mLockSettingChanges)
        return;
    LXQtFancyMenuCategoryPosition pos = LXQtFancyMenuCategoryPosition(this->ui->categoryViewPosCB->itemData(idx).toInt());
    bool value = (pos == LXQtFancyMenuCategoryPosition::Right);
    this->settings().setValue(QStringLiteral("categoriesAtRight"), value);
}
