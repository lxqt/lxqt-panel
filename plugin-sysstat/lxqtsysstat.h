/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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

#ifndef LXQTPANELSYSSTAT_H
#define LXQTPANELSYSSTAT_H

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtsysstatconfiguration.h"

#include <QLabel>


class LXQtSysStatTitle;
class LXQtSysStatContent;
class LXQtPanel;

namespace SysStat {
    class BaseStat;
}

class LXQtSysStat : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    LXQtSysStat(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtSysStat();

    virtual QWidget *widget() { return mWidget; }
    virtual QString themeId() const { return "SysStat"; }
    virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual bool isSeparate() const { return true; }

    QDialog *configureDialog();

    void realign();

protected slots:
    virtual void lateInit();
    virtual void settingsChanged();

private:
    QWidget *mWidget;
    LXQtSysStatTitle *mFakeTitle;
    LXQtSysStatContent *mContent;
    QSize mSize;
};

class LXQtSysStatTitle : public QLabel
{
    Q_OBJECT
public:
    LXQtSysStatTitle(QWidget *parent = NULL);
    ~LXQtSysStatTitle();

protected:
    bool event(QEvent *e);

signals:
    void fontChanged(QFont);
};

class LXQtSysStatContent : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor gridColor           READ gridColor           WRITE setGridColor)
    Q_PROPERTY(QColor titleColor          READ titleColor          WRITE setTitleColor)
    Q_PROPERTY(QColor cpuSystemColor      READ cpuSystemColor      WRITE setCpuSystemColor)
    Q_PROPERTY(QColor cpuUserColor        READ cpuUserColor        WRITE setCpuUserColor)
    Q_PROPERTY(QColor cpuNiceColor        READ cpuNiceColor        WRITE setCpuNiceColor)
    Q_PROPERTY(QColor cpuOtherColor       READ cpuOtherColor       WRITE setCpuOtherColor)
    Q_PROPERTY(QColor frequencyColor      READ frequencyColor      WRITE setFrequencyColor)
    Q_PROPERTY(QColor memAppsColor        READ memAppsColor        WRITE setMemAppsColor)
    Q_PROPERTY(QColor memBuffersColor     READ memBuffersColor     WRITE setMemBuffersColor)
    Q_PROPERTY(QColor memCachedColor      READ memCachedColor      WRITE setMemCachedColor)
    Q_PROPERTY(QColor swapUsedColor       READ swapUsedColor       WRITE setSwapUsedColor)
    Q_PROPERTY(QColor netReceivedColor    READ netReceivedColor    WRITE setNetReceivedColor)
    Q_PROPERTY(QColor netTransmittedColor READ netTransmittedColor WRITE setNetTransmittedColor)

public:
    LXQtSysStatContent(ILXQtPanelPlugin *plugin, QWidget *parent = NULL);
    ~LXQtSysStatContent();

    void updateSettings(const PluginSettings *);

#undef QSS_COLOUR
#define QSS_COLOUR(GETNAME, SETNAME) \
    QColor GETNAME##Color() const; \
    void SETNAME##Color(QColor value);

    QSS_COLOUR(grid,           setGrid)
    QSS_COLOUR(title,          setTitle)
    QSS_COLOUR(cpuSystem,      setCpuSystem)
    QSS_COLOUR(cpuUser,        setCpuUser)
    QSS_COLOUR(cpuNice,        setCpuNice)
    QSS_COLOUR(cpuOther,       setCpuOther)
    QSS_COLOUR(frequency,      setFrequency)
    QSS_COLOUR(memApps,        setMemApps)
    QSS_COLOUR(memBuffers,     setMemBuffers)
    QSS_COLOUR(memCached,      setMemCached)
    QSS_COLOUR(swapUsed,       setSwapUsed)
    QSS_COLOUR(netReceived,    setNetReceived)
    QSS_COLOUR(netTransmitted, setNetTransmitted)

#undef QSS_COLOUR

public slots:
    void setTitleFont(QFont value);
    void reset();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

protected slots:
    void cpuUpdate(float user, float nice, float system, float other, float frequencyRate, uint frequency);
    void cpuUpdate(float user, float nice, float system, float other);
    void memoryUpdate(float apps, float buffers, float cached);
    void swapUpdate(float used);
    void networkUpdate(unsigned received, unsigned transmitted);

private:
    void toolTipInfo(QString const & tooltip);

private:
    ILXQtPanelPlugin *mPlugin;

    SysStat::BaseStat *mStat;

    typedef struct ColorPalette
    {
        QColor gridColor;

        QColor titleColor;

        QColor cpuSystemColor;
        QColor cpuUserColor;
        QColor cpuNiceColor;
        QColor cpuOtherColor;
        QColor frequencyColor;

        QColor memAppsColor;
        QColor memBuffersColor;
        QColor memCachedColor;
        QColor swapUsedColor;

        QColor netReceivedColor;
        QColor netTransmittedColor;
    } ColorPalette;

    double mUpdateInterval;
    int mMinimalSize;

    int mGridLines;

    QString mTitleLabel;
    QFont mTitleFont;
    int mTitleFontPixelHeight;

    QString mDataType;

    QString mDataSource;

    bool mUseFrequency;

    int mNetMaximumSpeed;
    qreal mNetRealMaximumSpeed;
    bool mLogarithmicScale;
    int mLogScaleSteps;
    qreal mLogScaleMax;


    bool mUseThemeColors;
    ColorPalette mThemeColors;
    ColorPalette mSettingsColors;
    ColorPalette mColors;
    QColor mNetBothColor;


    int mHistoryOffset;
    QImage mHistoryImage;


    void clearLine();

    void mixNetColors();
    void updateTitleFontPixelHeight();
};


class LXQtSysStatLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LXQtSysStat(startupInfo);
    }
};

#endif // LXQTPANELSYSSTAT_H
