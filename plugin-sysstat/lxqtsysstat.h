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

#ifndef LXQTPANELSYSSTAT_H
#define LXQTPANELSYSSTAT_H

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtsysstatconfiguration.h"

#include <QLabel>


class LxQtSysStatTitle;
class LxQtSysStatContent;
class LxQtPanel;

namespace SysStat {
    class BaseStat;
}

class LxQtSysStat : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtSysStat(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtSysStat();

    virtual QWidget *widget() { return mWidget; }
    virtual QString themeId() const { return "SysStat"; }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual bool isSeparate() const { return true; }

    QDialog *configureDialog();

    void realign();

protected slots:
    virtual void lateInit();
    virtual void settingsChanged();

private:
    QWidget *mWidget;
    LxQtSysStatTitle *mFakeTitle;
    LxQtSysStatContent *mContent;
    QSize mSize;
};

class LxQtSysStatTitle : public QLabel
{
    Q_OBJECT
public:
    LxQtSysStatTitle(QWidget *parent = NULL);
    ~LxQtSysStatTitle();

protected:
    bool event(QEvent *e);

signals:
    void fontChanged(QFont);
};

class LxQtSysStatContent : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor gridColor           READ gridColour           WRITE setGridColour)
    Q_PROPERTY(QColor titleColor          READ titleColour          WRITE setTitleColour)
    Q_PROPERTY(QColor cpuSystemColor      READ cpuSystemColour      WRITE setCpuSystemColour)
    Q_PROPERTY(QColor cpuUserColor        READ cpuUserColour        WRITE setCpuUserColour)
    Q_PROPERTY(QColor cpuNiceColor        READ cpuNiceColour        WRITE setCpuNiceColour)
    Q_PROPERTY(QColor cpuOtherColor       READ cpuOtherColour       WRITE setCpuOtherColour)
    Q_PROPERTY(QColor frequencyColor      READ frequencyColour      WRITE setFrequencyColour)
    Q_PROPERTY(QColor memAppsColor        READ memAppsColour        WRITE setMemAppsColour)
    Q_PROPERTY(QColor memBuffersColor     READ memBuffersColour     WRITE setMemBuffersColour)
    Q_PROPERTY(QColor memCachedColor      READ memCachedColour      WRITE setMemCachedColour)
    Q_PROPERTY(QColor swapUsedColor       READ swapUsedColour       WRITE setSwapUsedColour)
    Q_PROPERTY(QColor netReceivedColor    READ netReceivedColour    WRITE setNetReceivedColour)
    Q_PROPERTY(QColor netTransmittedColor READ netTransmittedColour WRITE setNetTransmittedColour)

public:
    LxQtSysStatContent(ILxQtPanelPlugin *plugin, QWidget *parent = NULL);
    ~LxQtSysStatContent();

    void updateSettings(const QSettings *);

#undef QSS_COLOUR
#define QSS_COLOUR(GETNAME, SETNAME) \
    QColor GETNAME##Colour() const; \
    void SETNAME##Colour(QColor value);

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
    ILxQtPanelPlugin *mPlugin;

    SysStat::BaseStat *mStat;

    typedef struct ColourPalette
    {
        QColor gridColour;

        QColor titleColour;

        QColor cpuSystemColour;
        QColor cpuUserColour;
        QColor cpuNiceColour;
        QColor cpuOtherColour;
        QColor frequencyColour;

        QColor memAppsColour;
        QColor memBuffersColour;
        QColor memCachedColour;
        QColor swapUsedColour;

        QColor netReceivedColour;
        QColor netTransmittedColour;
    } ColourPalette;

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


    bool mUseThemeColours;
    ColourPalette mThemeColours;
    ColourPalette mSettingsColours;
    ColourPalette mColours;
    QColor mNetBothColour;


    int mHistoryOffset;
    QImage mHistoryImage;


    void clearLine();

    void mixNetColours();
    void updateTitleFontPixelHeight();
};


class LxQtSysStatLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LxQtSysStat(startupInfo);
    }
};

#endif // LXQTPANELSYSSTAT_H
