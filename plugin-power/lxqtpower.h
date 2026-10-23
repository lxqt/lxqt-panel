/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2026 LXQt team
 * Authors:
 *   envy-vain <340632583+envy-vain@users.noreply.github.com>
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

#ifndef LXQTPOWER_H
#define LXQTPOWER_H

#include <QElapsedTimer>
#include <QLabel>

class ILXQtPanelPlugin;

/*!
 * Shows the power draw of the machine as text: what the battery is delivering
 * (or receiving, while charging) and what the CPU package is using, both read
 * from sysfs (power_supply and powercap/RAPL). Either part is left out when the
 * kernel does not expose it.
 */
class LXQtPower : public QLabel
{
    Q_OBJECT

public:
    explicit LXQtPower(ILXQtPanelPlugin *plugin, QWidget *parent = nullptr);

    void settingsChanged();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void refresh();
    bool batteryPower(double &watts, QString &status);
    bool cpuPower(double &watts);

    ILXQtPanelPlugin *mPlugin;
    int mTimerId;
    bool mShowBattery;
    bool mShowCpu;

    QString mBatteryDir;
    QString mRaplDir;
    qint64 mLastEnergy;   // µJ, -1 until the first reading
    qint64 mLastNsecs;
    QElapsedTimer mClock;
};

#endif // LXQTPOWER_H
