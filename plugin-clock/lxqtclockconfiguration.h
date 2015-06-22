/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2011      Razor team
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


#ifndef LXQTCLOCKCONFIGURATION_H
#define LXQTCLOCKCONFIGURATION_H

#include <QDialog>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QLocale>
#include <QDateTime>

#include <LXQt/Settings>

namespace Ui {
    class LxQtClockConfiguration;
}

class LxQtClockConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit LxQtClockConfiguration(QSettings &settings, QWidget *parent = 0);
    ~LxQtClockConfiguration();

private:
    Ui::LxQtClockConfiguration *ui;
    QSettings &mSettings;
    LxQt::SettingsCache oldSettings;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

    /*
      Creates a date formats consistent with the region read from locale.
    */
    void createDateFormats();

private slots:
    /*
      Saves settings in conf file.
    */
    void saveSettings();
    void dialogButtonsAction(QAbstractButton *btn);
    void dateFormatActivated(int);

private:
    int mOldIndex;
    QString mCustomDateFormat;

    void addDateFormat(const QString &format);
};

#endif // LXQTCLOCKCONFIGURATION_H
