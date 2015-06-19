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


#ifndef LXQTSYSSTATCONFIGURATION_H
#define LXQTSYSSTATCONFIGURATION_H

#include <LXQt/Settings>

#include <QDialog>
#include <QAbstractButton>
#include <QMap>


namespace Ui {
    class LxQtSysStatConfiguration;
}

namespace SysStat {
    class BaseStat;
}

class LxQtSysStatColours;

class LxQtSysStatConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit LxQtSysStatConfiguration(QSettings *settings, QWidget *parent = 0);
    ~LxQtSysStatConfiguration();

public slots:
    void saveSettings();

    void on_typeCOB_currentIndexChanged(int);
    void on_maximumHS_valueChanged(int);
    void on_customColoursB_clicked();
    void on_buttons_clicked(QAbstractButton *);

    void coloursChanged();

signals:
    void maximumNetSpeedChanged(QString);

private:
    Ui::LxQtSysStatConfiguration *ui;
    QSettings *mSettings;
    LxQt::SettingsCache oldSettings;

    void loadSettings();

    SysStat::BaseStat *mStat;

    bool mLockSaving;

    LxQtSysStatColours *mColoursDialog;
};

#endif // LXQTSYSSTATCONFIGURATION_H
