/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014-2015 LXQt team
 *            2012      Razor team
 * Authors:
 *   Łukasz Twarduś <ltwardus@gmail.com>
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

#ifndef LXQTSENSORS_H
#define LXQTSENSORS_H

#include "sensors.h"
#include <QFrame>
#include <QProgressBar>
#include <QSet>
#include <QTimer>


class ProgressBar: public QProgressBar
{
    Q_OBJECT
public:
    ProgressBar(QWidget *parent = 0);

    QSize sizeHint() const;
};


class QSettings;
class ILxQtPanelPlugin;
class QBoxLayout;

class LxQtSensors : public QFrame
{
    Q_OBJECT
public:
    LxQtSensors(ILxQtPanelPlugin *plugin, QWidget* parent = 0);
    ~LxQtSensors();

    void settingsChanged();
    void realign();
public slots:
    void updateSensorReadings();
    void warningAboutHighTemperature();

private:
    ILxQtPanelPlugin *mPlugin;
    QBoxLayout *mLayout;
    QTimer mUpdateSensorReadingsTimer;
    QTimer mWarningAboutHighTemperatureTimer;
    Sensors mSensors;
    QList<Chip> mDetectedChips;
    QList<ProgressBar*> mTemperatureProgressBars;
    // With set we can handle updates in very easy way :)
    QSet<ProgressBar*> mHighTemperatureProgressBars;
    double celsiusToFahrenheit(double celsius);
    void initDefaultSettings();
    QSettings *mSettings;
};


#endif // LXQTSENSORS_H
