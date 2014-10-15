/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012-2013 Razor team
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

#ifndef LXQTPANELWORLDCLOCK_H
#define LXQTPANELWORLDCLOCK_H

#ifdef ICU_VERSION
#include <unicode/unistr.h>
#else
#include <QTimeZone>
#endif

#include <QLabel>

#include <LXQt/RotatedWidget>

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtworldclockconfiguration.h"


class ActiveLabel;
class QTimer;
class QDialog;
class LxQtWorldClockEventFilter;

#ifdef ICU_VERSION
namespace U_ICU_NAMESPACE {
    class Locale;
    class Calendar;
    class DateFormat;
}
#endif

class LxQtWorldClock : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtWorldClock(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtWorldClock();

    virtual QWidget *widget() { return mMainWidget; }
    virtual QString themeId() const { return "WorldClock"; }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    bool isSeparate() const { return true; }
    void activated(ActivationReason reason);

    virtual void settingsChanged();
    virtual void realign();
    QDialog *configureDialog();

private slots:
    void synchroTimeout();
    void timeout();
    void wheelScrolled(int);

private:
#ifdef ICU_VERSION
    static size_t instanceCounter;
#endif

    QWidget *mMainWidget;
    LxQt::RotatedWidget* mRotatedWidget;
    ActiveLabel *mContent;
    QDialog* mPopup;

    typedef enum FormatType
    {
        FORMAT__INVALID = -1,
        FORMAT_SHORT = 0,
        FORMAT_MEDIUM, // obsolete, use FORMAT_SHORT or FORMAT_CUSTOM
        FORMAT_LONG,
        FORMAT_FULL, // obsolete, use FORMAT_FULL or FORMAT_CUSTOM
        FORMAT_CUSTOM
    } FormatType;

    QTimer *mTimer;

    QStringList mTimeZones;
    QString mDefaultTimeZone;
    QString mActiveTimeZone;
    QString mCustomFormat;
    FormatType mFormatType;

    bool mAutoRotate;
#ifdef ICU_VERSION
    QString mDefaultLanguage;

    icu::Locale *mLocale;
    icu::Calendar *mCalendar;
    icu::DateFormat *mFormat;
    icu::UnicodeString mLastShownText;
#else
    QString mLastShownText;
    Qt::DateFormat mFormat;
#endif
    QLabel *mPopupContent;

    void updateFormat();
    void restartTimer(int);

    void updatePopupContent();
#ifdef ICU_VERSION
    void updateTimezone();
#else
    QString preformat(const QString &format, const QTimeZone &timeZone, const QDateTime& dateTime);
#endif
};


class ActiveLabel : public QLabel
{
Q_OBJECT

public:
    explicit ActiveLabel(QWidget * = NULL);

signals:
    void wheelScrolled(int);
    void leftMouseButtonClicked();
    void middleMouseButtonClicked();

protected:
    void wheelEvent(QWheelEvent *);
    void mouseReleaseEvent(QMouseEvent* event);
};

class LxQtWorldClockLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo)
    {
        return new LxQtWorldClock(startupInfo);
    }
};


#endif // LXQTPANELWORLDCLOCK_H
