/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012      Razor team
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


#ifndef LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H
#define LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H

#include <LXQt/Settings>

#include <QDialog>
#include <QAbstractButton>
#include <QFont>
#include <QMap>


namespace Ui {
    class LxQtWorldClockConfiguration;
}

class LxQtWorldClockConfigurationTimeZones;
class LxQtWorldClockConfigurationManualFormat;
class QTableWidgetItem;

class LxQtWorldClockConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit LxQtWorldClockConfiguration(QSettings *settings, QWidget *parent = NULL);
    ~LxQtWorldClockConfiguration();

public slots:
    void saveSettings();

private:
    Ui::LxQtWorldClockConfiguration *ui;
    QSettings *mSettings;
    LxQt::SettingsCache mOldSettings;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

private slots:
    /*
      Saves settings in conf file.
    */
    void dialogButtonsAction(QAbstractButton *);

    void timeFormatChanged(int);
    void dateGroupToggled(bool);
    void dateFormatChanged(int);
    void advancedFormatToggled(bool);
    void customiseManualFormatClicked();
    void manualFormatChanged();

    void updateTimeZoneButtons();
    void addTimeZone();
    void removeTimeZone();
    void setTimeZoneAsDefault();
    void editTimeZoneCustomName();
    void moveTimeZoneUp();
    void moveTimeZoneDown();

private:
    QString mDefaultTimeZone;

    bool mLockCascadeSettingChanges;

    LxQtWorldClockConfigurationTimeZones *mConfigurationTimeZones;
    LxQtWorldClockConfigurationManualFormat *mConfigurationManualFormat;

    QString mManualFormat;

    void setDefault(int);
    void setBold(QTableWidgetItem*, bool);
    void setBold(int row, bool value);
    int findTimeZone(const QString& timeZone);
};

#endif // LXQT_PANEL_WORLDCLOCK_CONFIGURATION_H
