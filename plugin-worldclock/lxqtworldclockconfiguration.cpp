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
#include "lxqtworldclockconfigurationmanualformat.h"


LxQtWorldClockConfiguration::LxQtWorldClockConfiguration(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LxQtWorldClockConfiguration),
    mSettings(settings),
    mOldSettings(settings),
    mLockCascadeSettingChanges(false),
    mConfigurationTimeZones(NULL),
    mConfigurationManualFormat(NULL)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName(QLatin1String("WorldClockConfigurationWindow"));
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));

    connect(ui->timeFormatCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->timeShowSecondsCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->timePadHourCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->timeAMPMCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->timezoneGB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->timezonePositionCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->timezoneFormatCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->dateGB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->datePositionCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->dateFormatCB, SIGNAL(currentIndexChanged(int)), SLOT(saveSettings()));
    connect(ui->dateShowYearCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->dateShowDoWCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->datePadDayCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->dateLongNamesCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->advancedManualGB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->customisePB, SIGNAL(clicked()), SLOT(customiseManualFormatClicked()));


    connect(ui->timeFormatCB, SIGNAL(currentIndexChanged(int)), SLOT(timeFormatChanged(int)));
    connect(ui->dateFormatCB, SIGNAL(currentIndexChanged(int)), SLOT(dateFormatChanged(int)));
    connect(ui->advancedManualGB, SIGNAL(toggled(bool)), SLOT(advancedFormatToggled(bool)));

    connect(ui->timeZonesLW, SIGNAL(itemSelectionChanged()), SLOT(updateTimeZoneButtons()));
    connect(ui->addPB, SIGNAL(clicked()), SLOT(addTimeZone()));
    connect(ui->removePB, SIGNAL(clicked()), SLOT(removeTimeZone()));
    connect(ui->setAsDefaultPB, SIGNAL(clicked()), SLOT(setTimeZoneAsDefault()));
    connect(ui->moveUpPB, SIGNAL(clicked()), SLOT(moveTimeZoneUp()));
    connect(ui->moveDownPB, SIGNAL(clicked()), SLOT(moveTimeZoneDown()));

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

    bool longTimeFormatSelected = false;

    QString formatType = mSettings->value(QLatin1String("formatType"), QString()).toString();
    QString dateFormatType = mSettings->value(QLatin1String("dateFormatType"), QString()).toString();
    bool advancedManual = mSettings->value(QLatin1String("useAdvancedManualFormat"), false).toBool();
    mManualFormat = mSettings->value(QLatin1String("customFormat"), tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'")).toString();

    // backward compatibility
    if (formatType == QLatin1String("custom"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = true;
    }
    else if (formatType == QLatin1String("short"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = false;
    }
    else if ((formatType == QLatin1String("full")) ||
             (formatType == QLatin1String("long")) ||
             (formatType == QLatin1String("medium")))
    {
        formatType = QLatin1String("long-timeonly");
        dateFormatType = QLatin1String("long");
        advancedManual = false;
    }


    if (formatType == QLatin1String("short-timeonly"))
        ui->timeFormatCB->setCurrentIndex(0);
    else if (formatType == QLatin1String("long-timeonly"))
    {
        ui->timeFormatCB->setCurrentIndex(1);
        longTimeFormatSelected = true;
    }
    else // if (formatType == QLatin1String("custom-timeonly"))
        ui->timeFormatCB->setCurrentIndex(2);

    ui->timeShowSecondsCB->setChecked(mSettings->value(QLatin1String("timeShowSeconds"), false).toBool() ? Qt::Checked : Qt:: Unchecked);
    ui->timePadHourCB->setChecked(mSettings->value(QLatin1String("timePadHour"), false).toBool() ? Qt::Checked : Qt:: Unchecked);
    ui->timeAMPMCB->setChecked(mSettings->value(QLatin1String("timeAMPM"), false).toBool() ? Qt::Checked : Qt:: Unchecked);

    // timezone
    ui->timezoneGB->setChecked((mSettings->value(QLatin1String("showTimezone"), false).toBool() && !longTimeFormatSelected) ? Qt::Checked : Qt:: Unchecked);

    QString timezonePosition = mSettings->value(QLatin1String("timezonePosition"), QString()).toString();
    if (timezonePosition == QLatin1String("above"))
        ui->timezonePositionCB->setCurrentIndex(1);
    else if (timezonePosition == QLatin1String("before"))
        ui->timezonePositionCB->setCurrentIndex(2);
    else if (timezonePosition == QLatin1String("after"))
        ui->timezonePositionCB->setCurrentIndex(3);
    else // if (timezonePosition == QLatin1String("below"))
        ui->timezonePositionCB->setCurrentIndex(0);

    QString timezoneFormatType = mSettings->value(QLatin1String("timezoneFormatType"), QString()).toString();
    if (timezoneFormatType == QLatin1String("short"))
        ui->timezoneFormatCB->setCurrentIndex(0);
    else if (timezoneFormatType == QLatin1String("long"))
        ui->timezoneFormatCB->setCurrentIndex(1);
    else if (timezoneFormatType == QLatin1String("offset"))
        ui->timezoneFormatCB->setCurrentIndex(2);
    else if (timezoneFormatType == QLatin1String("abbreviation"))
        ui->timezoneFormatCB->setCurrentIndex(3);
    else // if (timezoneFormatType == QLatin1String("iana"))
        ui->timezoneFormatCB->setCurrentIndex(4);

    // date
    ui->dateGB->setChecked(mSettings->value(QLatin1String("showDate"), false).toBool() ? Qt::Checked : Qt:: Unchecked);

    QString datePosition = mSettings->value(QLatin1String("datePosition"), QString()).toString();
    if (datePosition == QLatin1String("above"))
        ui->datePositionCB->setCurrentIndex(1);
    else if (datePosition == QLatin1String("before"))
        ui->datePositionCB->setCurrentIndex(2);
    else if (datePosition == QLatin1String("after"))
        ui->datePositionCB->setCurrentIndex(3);
    else // if (datePosition == QLatin1String("below"))
        ui->datePositionCB->setCurrentIndex(0);

    if (dateFormatType == QLatin1String("short"))
        ui->dateFormatCB->setCurrentIndex(0);
    else if (dateFormatType == QLatin1String("long"))
        ui->dateFormatCB->setCurrentIndex(1);
    else if (dateFormatType == QLatin1String("iso"))
        ui->dateFormatCB->setCurrentIndex(2);
    else // if (dateFormatType == QLatin1String("custom"))
        ui->dateFormatCB->setCurrentIndex(3);

    ui->dateShowYearCB->setChecked(mSettings->value(QLatin1String("dateShowYear"), false).toBool() ? Qt::Checked : Qt:: Unchecked);
    ui->dateShowDoWCB->setChecked(mSettings->value(QLatin1String("dateShowDoW"), false).toBool() ? Qt::Checked : Qt:: Unchecked);
    ui->datePadDayCB->setChecked(mSettings->value(QLatin1String("datePadDay"), false).toBool() ? Qt::Checked : Qt:: Unchecked);
    ui->dateLongNamesCB->setChecked(mSettings->value(QLatin1String("dateLongNames"), false).toBool() ? Qt::Checked : Qt:: Unchecked);


    ui->advancedManualGB->setChecked(advancedManual ? Qt::Checked : Qt:: Unchecked);


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


    ui->autorotateCB->setChecked(mSettings->value("autoRotate", true).toBool());


    mLockCascadeSettingChanges = false;
}

void LxQtWorldClockConfiguration::saveSettings()
{
    if (mLockCascadeSettingChanges)
        return;

    QString formatType;
    switch (ui->timeFormatCB->currentIndex())
    {
    case 0:
        formatType = QLatin1String("short-timeonly");
        break;

    case 1:
        formatType = QLatin1String("long-timeonly");
        break;

    case 2:
        formatType = QLatin1String("custom-timeonly");
        break;
    }
    mSettings->setValue(QLatin1String("formatType"), formatType);

    mSettings->setValue(QLatin1String("timeShowSeconds"), ui->timeShowSecondsCB->isChecked());
    mSettings->setValue(QLatin1String("timePadHour"), ui->timePadHourCB->isChecked());
    mSettings->setValue(QLatin1String("timeAMPM"), ui->timeAMPMCB->isChecked());

    mSettings->setValue(QLatin1String("showTimezone"), ui->timezoneGB->isChecked());

    QString timezonePosition;
    switch (ui->timezonePositionCB->currentIndex())
    {
    case 0:
        timezonePosition = QLatin1String("below");
        break;

    case 1:
        timezonePosition = QLatin1String("above");
        break;

    case 2:
        timezonePosition = QLatin1String("before");
        break;

    case 3:
        timezonePosition = QLatin1String("after");
        break;
    }
    mSettings->setValue(QLatin1String("timezonePosition"), timezonePosition);

    QString timezoneFormatType;
    switch (ui->timezoneFormatCB->currentIndex())
    {
    case 0:
        timezoneFormatType = QLatin1String("short");
        break;

    case 1:
        timezoneFormatType = QLatin1String("long");
        break;

    case 2:
        timezoneFormatType = QLatin1String("offset");
        break;

    case 3:
        timezoneFormatType = QLatin1String("abbreviation");
        break;

    case 4:
        timezoneFormatType = QLatin1String("iana");
        break;
    }
    mSettings->setValue(QLatin1String("timezoneFormatType"), timezoneFormatType);

    mSettings->setValue(QLatin1String("showDate"), ui->dateGB->isChecked());

    QString datePosition;
    switch (ui->datePositionCB->currentIndex())
    {
    case 0:
        datePosition = QLatin1String("below");
        break;

    case 1:
        datePosition = QLatin1String("above");
        break;

    case 2:
        datePosition = QLatin1String("before");
        break;

    case 3:
        datePosition = QLatin1String("after");
        break;
    }
    mSettings->setValue(QLatin1String("datePosition"), datePosition);

    QString dateFormatType;
    switch (ui->dateFormatCB->currentIndex())
    {
    case 0:
        dateFormatType = QLatin1String("short");
        break;

    case 1:
        dateFormatType = QLatin1String("long");
        break;

    case 2:
        dateFormatType = QLatin1String("iso");
        break;

    case 3:
        dateFormatType = QLatin1String("custom");
        break;
    }
    mSettings->setValue(QLatin1String("dateFormatType"), dateFormatType);

    mSettings->setValue(QLatin1String("dateShowYear"), ui->dateShowYearCB->isChecked());
    mSettings->setValue(QLatin1String("dateShowDoW"), ui->dateShowDoWCB->isChecked());
    mSettings->setValue(QLatin1String("datePadDay"), ui->datePadDayCB->isChecked());
    mSettings->setValue(QLatin1String("dateLongNames"), ui->dateLongNamesCB->isChecked());

    mSettings->setValue(QLatin1String("customFormat"), mManualFormat);


    int size = ui->timeZonesLW->count();
    mSettings->beginWriteArray(QLatin1String("timeZones"), size);
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        mSettings->setValue(QLatin1String("timeZone"), ui->timeZonesLW->item(i)->text());
    }
    mSettings->endArray();

    mSettings->setValue(QLatin1String("defaultTimeZone"), mDefaultTimeZone);


    mSettings->setValue(QLatin1String("useAdvancedManualFormat"), ui->advancedManualGB->isChecked());


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

void LxQtWorldClockConfiguration::timeFormatChanged(int index)
{
    bool longTimeFormatSelected = index == 1;
    bool customTimeFormatSelected = index == 2;
    ui->timeCustomW->setEnabled(customTimeFormatSelected);
    ui->timezoneGB->setEnabled(!longTimeFormatSelected);
}

void LxQtWorldClockConfiguration::dateFormatChanged(int index)
{
    ui->dateCustomW->setEnabled(index == ui->dateFormatCB->count() - 1);
}

void LxQtWorldClockConfiguration::advancedFormatToggled(bool on)
{
    bool longTimeFormatSelected = ui->timeFormatCB->currentIndex() == 1;
    ui->timeGB->setEnabled(!on);
    ui->timezoneGB->setEnabled(!on && !longTimeFormatSelected);
    ui->dateGB->setEnabled(!on);
}

void LxQtWorldClockConfiguration::customiseManualFormatClicked()
{
    if (!mConfigurationManualFormat)
    {
        mConfigurationManualFormat = new LxQtWorldClockConfigurationManualFormat(this);
        connect(mConfigurationManualFormat, SIGNAL(manualFormatChanged()), this, SLOT(manualFormatChanged()));
    }

    mConfigurationManualFormat->setManualFormat(mManualFormat);

    QString oldManualFormat = mManualFormat;

    mManualFormat = (mConfigurationManualFormat->exec() == QDialog::Accepted) ? mConfigurationManualFormat->manualFormat() : oldManualFormat;

    saveSettings();
}

void LxQtWorldClockConfiguration::manualFormatChanged()
{
    mManualFormat = mConfigurationManualFormat->manualFormat();
    saveSettings();
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
