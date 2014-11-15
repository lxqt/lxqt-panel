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

#include "ui_lxqtworldclockconfiguration.h"

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
    setObjectName(QLatin1String("WorldClockConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->timeZonesLW, SIGNAL(itemSelectionChanged()), SLOT(updateTimeZoneButtons()));
    connect(ui->addPB, SIGNAL(clicked()), SLOT(addTimeZone()));
    connect(ui->removePB, SIGNAL(clicked()), SLOT(removeTimeZone()));
    connect(ui->setAsDefaultPB, SIGNAL(clicked()), SLOT(setTimeZoneAsDefault()));
    connect(ui->moveUpPB, SIGNAL(clicked()), SLOT(moveTimeZoneUp()));
    connect(ui->moveDownPB, SIGNAL(clicked()), SLOT(moveTimeZoneDown()));

    connect(ui->shortFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->longFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->shortTimeOnlyFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
    connect(ui->longTimeOnlyFormatRB, SIGNAL(toggled(bool)), SLOT(saveSettings()));
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

    int size = mSettings->beginReadArray(QLatin1String("timeZones"));
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        ui->timeZonesLW->addItem(mSettings->value(QLatin1String("timeZone"), QString()).toString());
    }
    mSettings->endArray();

    mDefaultTimeZone = mSettings->value("defaultTimeZone", QString()).toString();
    if (mDefaultTimeZone.isEmpty() && ui->timeZonesLW->count())
        mDefaultTimeZone = ui->timeZonesLW->item(0)->text();

    if (ui->timeZonesLW->count())
        setBold(ui->timeZonesLW->findItems(mDefaultTimeZone, Qt::MatchExactly)[0], true);

    ui->customFormatPTE->setPlainText(mSettings->value(QLatin1String("customFormat"), tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'")).toString());

    QString formatType = mSettings->value(QLatin1String("formatType"), QString()).toString();
    if (formatType == QLatin1String("custom"))
        ui->customFormatRB->setChecked(true);
    else if (formatType == QLatin1String("full"))
        ui->longFormatRB->setChecked(true);
    else if (formatType == QLatin1String("long"))
        ui->longFormatRB->setChecked(true);
    else if (formatType == QLatin1String("medium"))
        ui->shortFormatRB->setChecked(true);
    else if (formatType == QLatin1String("short-timeonly"))
        ui->shortTimeOnlyFormatRB->setChecked(true);
    else if (formatType == QLatin1String("long-timeonly"))
        ui->longTimeOnlyFormatRB->setChecked(true);
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
    mSettings->beginWriteArray(QLatin1String("timeZones"), size);
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        mSettings->setValue(QLatin1String("timeZone"), ui->timeZonesLW->item(i)->text());
    }
    mSettings->endArray();

    mSettings->setValue(QLatin1String("defaultTimeZone"), mDefaultTimeZone);

    mSettings->setValue(QLatin1String("customFormat"), ui->customFormatPTE->toPlainText());

    if (ui->customFormatRB->isChecked())
        mSettings->setValue(QLatin1String("formatType"), QLatin1String("custom"));
    else if (ui->longFormatRB->isChecked())
        mSettings->setValue(QLatin1String("formatType"), QLatin1String("long"));
    else if (ui->longTimeOnlyFormatRB->isChecked())
        mSettings->setValue(QLatin1String("formatType"), QLatin1String("long-timeonly"));
    else if (ui->shortTimeOnlyFormatRB->isChecked())
        mSettings->setValue(QLatin1String("formatType"), QLatin1String("short-timeonly"));
    else
        mSettings->setValue(QLatin1String("formatType"), QLatin1String("short"));

    mSettings->setValue(QLatin1String("autoRotate"), ui->autorotateCB->isChecked());
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
        QString timeZone = mConfigurationTimeZones->timeZone();
        if (ui->timeZonesLW->findItems(timeZone, Qt::MatchExactly).empty())
        {
            QListWidgetItem *item = new QListWidgetItem(timeZone);
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
