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

#include "lxqtpower.h"
#include "../panel/ilxqtpanelplugin.h"
#include "../panel/pluginsettings.h"

#include <QDir>
#include <QFile>

static const QString powerSupplyDir = QStringLiteral("/sys/class/power_supply");
static const QString raplDir = QStringLiteral("/sys/class/powercap/intel-rapl:0"); // AMD exposes RAPL here too

static QString readText(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    return QString::fromLatin1(f.readLine()).trimmed();
}

// -1 when the file is missing or unreadable (e.g. energy_uj on kernels that keep it root-only)
static qint64 readNumber(const QString &path)
{
    bool ok = false;
    qint64 v = readText(path).toLongLong(&ok);
    return ok ? v : -1;
}

// The first system battery. Wireless mice and keyboards show up as type Battery too, with scope Device.
static QString findBattery()
{
    const QStringList entries = QDir(powerSupplyDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString &name : entries)
    {
        const QString dir = powerSupplyDir + QLatin1Char('/') + name;
        if (readText(dir + QStringLiteral("/type")) == QLatin1String("Battery")
            && readText(dir + QStringLiteral("/scope")) != QLatin1String("Device"))
            return dir;
    }
    return QString();
}

LXQtPower::LXQtPower(ILXQtPanelPlugin *plugin, QWidget *parent) :
    QLabel(parent),
    mPlugin(plugin),
    mTimerId(-1),
    mShowBattery(true),
    mShowCpu(true),
    mBatteryDir(findBattery()),
    mRaplDir(QFile::exists(raplDir + QStringLiteral("/energy_uj")) ? raplDir : QString()),
    mLastEnergy(-1),
    mLastNsecs(0)
{
    setObjectName(QStringLiteral("Power"));
    setAlignment(Qt::AlignCenter);
    mClock.start();
    settingsChanged();
}

void LXQtPower::settingsChanged()
{
    if (mTimerId != -1)
        killTimer(mTimerId);

    mShowBattery = mPlugin->settings()->value(QStringLiteral("showBattery"), true).toBool();
    mShowCpu = mPlugin->settings()->value(QStringLiteral("showCpu"), true).toBool();
    const int interval = mPlugin->settings()->value(QStringLiteral("updateInterval"), 2000).toInt();

    refresh();
    mTimerId = startTimer(qMax(interval, 500));
}

void LXQtPower::timerEvent(QTimerEvent * /*event*/)
{
    refresh();
}

bool LXQtPower::batteryPower(double &watts, QString &status)
{
    if (mBatteryDir.isEmpty())
        return false;

    qint64 uw = readNumber(mBatteryDir + QStringLiteral("/power_now"));
    if (uw < 0)
    {
        // some batteries only report current and voltage
        const qint64 ua = readNumber(mBatteryDir + QStringLiteral("/current_now"));
        const qint64 uv = readNumber(mBatteryDir + QStringLiteral("/voltage_now"));
        if (ua < 0 || uv < 0)
            return false;
        uw = ua * uv / 1000000;
    }
    watts = uw / 1e6;
    status = readText(mBatteryDir + QStringLiteral("/status"));
    return true;
}

// Average CPU package power since the previous call, from the RAPL energy counter.
bool LXQtPower::cpuPower(double &watts)
{
    if (mRaplDir.isEmpty())
        return false;

    const qint64 energy = readNumber(mRaplDir + QStringLiteral("/energy_uj"));
    const qint64 nsecs = mClock.nsecsElapsed();
    if (energy < 0)
        return false;

    bool ok = false;
    if (mLastEnergy >= 0 && nsecs > mLastNsecs)
    {
        qint64 delta = energy - mLastEnergy;
        if (delta < 0) // the counter wrapped
            delta += readNumber(mRaplDir + QStringLiteral("/max_energy_range_uj"));
        watts = delta / 1e6 / ((nsecs - mLastNsecs) / 1e9);
        ok = delta >= 0;
    }
    mLastEnergy = energy;
    mLastNsecs = nsecs;
    return ok;
}

void LXQtPower::refresh()
{
    QStringList text, tip;
    double watts;
    QString status;

    if (mShowBattery && batteryPower(watts, status))
    {
        const QString w = QString::number(watts, 'f', 1);
        if (status == QLatin1String("Charging"))
        {
            text << tr("+%1 W").arg(w);
            tip << tr("Battery: charging at %1 W").arg(w);
        }
        else
        {
            text << tr("%1 W").arg(w);
            tip << (status == QLatin1String("Discharging") ? tr("Battery: discharging at %1 W").arg(w)
                                                           : tr("Battery: %1 W").arg(w));
        }
    }

    if (mShowCpu && cpuPower(watts))
    {
        const QString w = QString::number(watts, 'f', 1);
        text << tr("CPU %1 W").arg(w);
        tip << tr("CPU package: %1 W").arg(w);
    }

    setText(text.join(QStringLiteral("  ")));
    setToolTip(tip.isEmpty() ? tr("No power readings available") : tip.join(QLatin1Char('\n')));
}
