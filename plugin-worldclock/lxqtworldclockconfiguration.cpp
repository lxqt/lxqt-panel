/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 *            2014 LXQt team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#include "lxqtworldclockconfiguration.h"

#ifdef ICU_VERSION
#include "ui_lxqtworldclockconfiguration_icu.h"
#else
#include "ui_lxqtworldclockconfiguration.h"
#endif

#include "lxqtworldclockconfigurationtimezones.h"


LxQtWorldClockConfiguration::LxQtWorldClockConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LxQtWorldClockConfiguration),
    mSettings(settings),
    mOldSettings(settings),
    mLockCascadeSettingChanges(false),
    mConfigurationTimeZones(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("WorldClockConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->timeZonesLW, SIGNAL(itemSelectionChanged()), SLOT(updateTimeZoneButtons()));
    connect(ui->addPB, SIGNAL(clicked()), SLOT(addTimeZone()));
    connect(ui->removePB, SIGNAL(clicked()), SLOT(removeTimeZone()));
    connect(ui->setAsDefaultPB, SIGNAL(clicked()), SLOT(setTimeZoneAsDefault()));
    connect(ui->moveUpPB, SIGNAL(clicked()), SLOT(moveTimeZoneUp()));
    connect(ui->moveDownPB, SIGNAL(clicked()), SLOT(moveTimeZoneDown()));

    connect(ui->shortFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
#ifdef ICU_VERSION
    connect(ui->mediumFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
#endif
    connect(ui->longFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
#ifdef ICU_VERSION
    connect(ui->fullFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
#endif
    connect(ui->customFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->customFormatPTE, SIGNAL(textChanged()), SLOT(saveSettings()));

    connect(ui->autorotateCB, SIGNAL(clicked()), SLOT(saveSettings()));

    loadSettings();
}

LxQtWorldClockConfiguration::~LxQtWorldClockConfiguration()
{
    delete ui;
}

void LxQtWorldClockConfiguration::loadSettings()
{
    mLockCascadeSettingChanges = true;

    ui->timeZonesLW->clear();

    int size = mSettings->beginReadArray("timeZones");
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        ui->timeZonesLW->addItem(mSettings->value("timeZone", QString()).toString());
    }
    mSettings->endArray();

    mDefaultTimeZone = mSettings->value("defaultTimeZone", QString()).toString();
    if (mDefaultTimeZone.isEmpty() && ui->timeZonesLW->count())
        mDefaultTimeZone = ui->timeZonesLW->item(0)->text();

    if (ui->timeZonesLW->count())
        setBold(ui->timeZonesLW->findItems(mDefaultTimeZone, Qt::MatchExactly)[0], true);

#ifdef ICU_VERSION
    ui->customFormatPTE->setPlainText(mSettings->value("customFormat", QString("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'eee, d MMM yyyy'<br/>'VVVV'</font>'")).toString());
#else
    ui->customFormatPTE->setPlainText(mSettings->value("customFormat", QString("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'")).toString());
#endif

    QString formatType = mSettings->value("formatType", QString()).toString();
    if (formatType == "custom")
        ui->customFormatRB->setChecked(true);
    else if (formatType == "full")
#ifdef ICU_VERSION
        ui->fullFormatRB->setChecked(true);
#else
        ui->longFormatRB->setChecked(true); // old 'full' is 'long' now
#endif
    else if (formatType == "long")
        ui->longFormatRB->setChecked(true);
    else if (formatType == "medium")
#ifdef ICU_VERSION
        ui->mediumFormatRB->setChecked(true);
#else
        ui->shortFormatRB->setChecked(true); // old 'medium' is 'short' now
#endif
    else
        ui->shortFormatRB->setChecked(true);

    ui->autorotateCB->setChecked(mSettings->value("autoRotate", true).toBool());

    mLockCascadeSettingChanges = false;
}

void LxQtWorldClockConfiguration::saveSettings()
{
    if (mLockCascadeSettingChanges)
        return;

    int size = ui->timeZonesLW->count();
    mSettings->beginWriteArray("timeZones", size);
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        mSettings->setValue("timeZone", ui->timeZonesLW->item(i)->text());
    }
    mSettings->endArray();

    mSettings->setValue("defaultTimeZone", mDefaultTimeZone);

    mSettings->setValue("customFormat", ui->customFormatPTE->toPlainText());

    if (ui->customFormatRB->isChecked())
        mSettings->setValue("formatType", "custom");
#ifdef ICU_VERSION
    else if (ui->fullFormatRB->isChecked())
        mSettings->setValue("formatType", "full");
#endif
    else if (ui->longFormatRB->isChecked())
        mSettings->setValue("formatType", "long");
#ifdef ICU_VERSION
    else if (ui->mediumFormatRB->isChecked())
        mSettings->setValue("formatType", "medium");
#endif
    else
        mSettings->setValue("formatType", "short");

    mSettings->setValue("autoRotate", ui->autorotateCB->isChecked());
}

void LxQtWorldClockConfiguration::dialogButtonsAction(QAbstractButton *button)
{
    if (ui->buttons->buttonRole(button) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
        close();
}

void LxQtWorldClockConfiguration::updateTimeZoneButtons()
{
    int selectedCount = ui->timeZonesLW->selectedItems().count();
    int allCount = ui->timeZonesLW->count();

    ui->removePB->setEnabled(selectedCount != 0);
    bool canSetAsDefault = (selectedCount == 1);
    if (canSetAsDefault)
        canSetAsDefault = (ui->timeZonesLW->selectedItems()[0]->text() != mDefaultTimeZone);

    bool canMoveUp = false;
    bool canMoveDown = false;
    if ((selectedCount != 0) && (selectedCount != allCount))
    {
        bool skipBottom = true;
        for (int i = allCount - 1; i >= 0; --i)
        {
            if (ui->timeZonesLW->item(i)->isSelected())
            {
                if (!skipBottom)
                {
                    canMoveDown = true;
                    break;
                }
            }
            else
                skipBottom = false;
        }

        bool skipTop = true;
        for (int i = 0; i < allCount; ++i)
        {
            if (ui->timeZonesLW->item(i)->isSelected())
            {
                if (!skipTop)
                {
                    canMoveUp = true;
                    break;
                }
            }
            else
                skipTop = false;
        }
    }
    ui->setAsDefaultPB->setEnabled(canSetAsDefault);
    ui->moveUpPB->setEnabled(canMoveUp);
    ui->moveDownPB->setEnabled(canMoveDown);
}

void LxQtWorldClockConfiguration::addTimeZone()
{
    if (!mConfigurationTimeZones)
        mConfigurationTimeZones = new LxQtWorldClockConfigurationTimeZones(this);

    if (mConfigurationTimeZones->updateAndExec() == QDialog::Accepted)
    {
        if (ui->timeZonesLW->findItems(mConfigurationTimeZones->timeZone(), Qt::MatchExactly).empty())
        {
            QListWidgetItem *item = new QListWidgetItem(mConfigurationTimeZones->timeZone());
            ui->timeZonesLW->addItem(item);
            if (mDefaultTimeZone.isEmpty())
                setDefault(item);
        }
    }

    saveSettings();
}

void LxQtWorldClockConfiguration::removeTimeZone()
{
    foreach (QListWidgetItem *item, ui->timeZonesLW->selectedItems())
    {
        if (item->text() == mDefaultTimeZone)
            mDefaultTimeZone.clear();
        delete item;
    }
    if ((mDefaultTimeZone.isEmpty()) && ui->timeZonesLW->count())
        setDefault(ui->timeZonesLW->item(0));

    saveSettings();
}

void LxQtWorldClockConfiguration::setBold(QListWidgetItem *item, bool value)
{
    if (item)
    {
        QFont font = item->font();
        font.setBold(value);
        item->setFont(font);
    }
}

void LxQtWorldClockConfiguration::setDefault(QListWidgetItem *item)
{
    setBold(item, true);
    mDefaultTimeZone = item->text();
}

void LxQtWorldClockConfiguration::setTimeZoneAsDefault()
{
    setBold(ui->timeZonesLW->findItems(mDefaultTimeZone, Qt::MatchExactly)[0], false);

    setDefault(ui->timeZonesLW->selectedItems()[0]);

    saveSettings();
}

void LxQtWorldClockConfiguration::moveTimeZoneUp()
{
    int m = ui->timeZonesLW->count();
    bool skipTop = true;
    for (int i = 0; i < m; ++i)
    {
        if (ui->timeZonesLW->item(i)->isSelected())
        {
            if (!skipTop)
            {
                ui->timeZonesLW->insertItem(i - 1, ui->timeZonesLW->takeItem(i));
                ui->timeZonesLW->item(i - 1)->setSelected(true);
            }
        }
        else
            skipTop = false;
    }

    saveSettings();
}

void LxQtWorldClockConfiguration::moveTimeZoneDown()
{
    int m = ui->timeZonesLW->count();
    bool skipBottom = true;
    for (int i = m - 1; i >= 0; --i)
    {
        if (ui->timeZonesLW->item(i)->isSelected())
        {
            if (!skipBottom)
            {
                ui->timeZonesLW->insertItem(i + 1, ui->timeZonesLW->takeItem(i));
                ui->timeZonesLW->item(i + 1)->setSelected(true);
            }
        }
        else
            skipBottom = false;
    }

    saveSettings();
}
