/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#ifndef LXQTKBINDICATORCONFIGURATION_H
#define LXQTKBINDICATORCONFIGURATION_H

#include <LXQt/Settings>

#include <QtGui/QDialog>
#include <QtGui/QAbstractButton>


namespace Ui {
    class LxQtKbIndicatorConfiguration;
}

class LxQtKbIndicatorConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit LxQtKbIndicatorConfiguration(QSettings *settings, QWidget *parent = 0);
    ~LxQtKbIndicatorConfiguration();

public slots:
    void saveSettings();

    void selectColour(const QString &);
    void on_typeCOB_currentIndexChanged(int);
    void on_sourceCOB_currentIndexChanged(int);
    void on_maximumHS_valueChanged(int);
    void on_fontB_clicked();


private:
    Ui::LxQtKbIndicatorConfiguration *ui;
    QSettings *mSettings;
    LxQt::SettingsCache oldSettings;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

    bool lockSaving;

private slots:
    /*
      Saves settings in conf file.
    */
    void dialogButtonsAction(QAbstractButton *btn);
};

#endif // LXQTKBINDICATORCONFIGURATION_H
