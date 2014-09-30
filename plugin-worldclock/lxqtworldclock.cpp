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

#include "lxqtworldclock.h"

#ifdef ICU_VERSION
#include <unicode/locid.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>
#include <unicode/smpdtfmt.h>
#include <unicode/timezone.h>
#include <unicode/uclean.h>
#endif

#include <QLocale>
#include <QTimer>
#include <QScopedArrayPointer>
#include <QDate>
//#include <QDebug>
#include <QWheelEvent>
#include <QCalendarWidget>
#include <QDesktopWidget>
#include <QDialog>
#include <QHBoxLayout>

#include <time.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(panelworldclock, LxQtWorldClockLibrary)
#endif

#ifdef ICU_VERSION
size_t LxQtWorldClock::instanceCounter = 0;


static QString ICU_to_Qt(const icu::UnicodeString &string)
{
    UErrorCode status = U_ZERO_ERROR;

    int32_t len32 = string.countChar32();
    QScopedArrayPointer<UChar32> uch32(new UChar32[len32]);
    string.toUTF32(uch32.data(), len32, status);
    QScopedArrayPointer<uint> ui32(new uint[len32]);
    for (int32_t i = 0; i < len32; ++i)
        ui32[i] = uch32[i];
    return QString::fromUcs4(ui32.data(), len32);
}

static icu::UnicodeString Qt_to_ICU(const QString &string)
{
    QVector<uint> ucs = string.toUcs4();
    int len = ucs.size();
    QScopedArrayPointer<UChar32> uch32(new UChar32[len]);
    for (int i = 0; i < len; ++i)
        uch32[i] = ucs[i];
    return icu::UnicodeString::fromUTF32(uch32.data(), len);
}
#endif


LxQtWorldClock::LxQtWorldClock(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mPopup(NULL),
    mTimer(new QTimer(this)),
    mFormatType(FORMAT__INVALID),
    mAutoRotate(true),
#ifdef ICU_VERSION
    mLocale(NULL),
    mCalendar(NULL),
    mFormat(NULL),
#else
    mFormat(Qt::TextDate),
#endif
    mPopupContent(NULL)
{
    mMainWidget = new QWidget();
    mContent = new ActiveLabel();
    mRotatedWidget = new LxQt::RotatedWidget(*mContent, mMainWidget);

    mRotatedWidget->setTransferWheelEvent(true);

    QVBoxLayout *borderLayout = new QVBoxLayout(mMainWidget);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->addWidget(mRotatedWidget, 0, Qt::AlignCenter);

    mContent->setObjectName("WorldClockContent");

    mContent->setAlignment(Qt::AlignCenter);

#ifdef ICU_VERSION
    mLocale = new icu::Locale();
    mDefaultLanguage = QString(mLocale->getLanguage());
#endif

    settingsChanged();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

    connect(mContent, SIGNAL(wheelScrolled(int)), SLOT(wheelScrolled(int)));

#ifdef ICU_VERSION
    ++instanceCounter;
#endif
}

LxQtWorldClock::~LxQtWorldClock()
{
    delete mMainWidget;

#ifdef ICU_VERSION
    delete mFormat;
    delete mCalendar;
    delete mLocale;

    --instanceCounter;
    if (!instanceCounter)
        u_cleanup();
#endif
}

void LxQtWorldClock::timeout()
{
#ifdef ICU_VERSION
    if (mFormat)
    {
        UErrorCode status = U_ZERO_ERROR;
        UnicodeString str;

        UDate now = Calendar::getNow();

        if ((mTimer->interval() >= 1000) && ((static_cast<long long>(now) % 1000) > 200))
            restartTimer(mTimer->interval());

        mFormat->format(now, str, status);
        if (U_FAILURE(status))
            qDebug() << "timeout: Calendar::getNow() status = " << status;

#else
        QDateTime now = QDateTime::currentDateTime();

        QTimeZone timeZone(mActiveTimeZone.toLatin1());
        QDateTime tzNow = now.toTimeZone(timeZone);
        QString str;
        if (mFormat == -1) // custom
            str = tzNow.toString(preformat(mCustomFormat, timeZone, tzNow));
        else
            str = tzNow.toString(mFormat);
#endif
        if (str != mLastShownText)
        {
#ifdef ICU_VERSION
            mContent->setText(ICU_to_Qt(str));
#else
            mContent->setText(str);
#endif
            mLastShownText = str;

            mRotatedWidget->adjustContentSize();
            mRotatedWidget->update();

            updatePopupContent();
        }
#ifdef ICU_VERSION
    }
#endif
}

void LxQtWorldClock::updateFormat()
{
#ifdef ICU_VERSION
    if (mFormat)
        delete mFormat;
#endif

    int timerInterval = 0;

    switch (mFormatType)
    {
    case FORMAT_CUSTOM:
    {
#ifdef ICU_VERSION
        UErrorCode status = U_ZERO_ERROR;
        mFormat = new icu::SimpleDateFormat(Qt_to_ICU(mCustomFormat), *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateFormat: SimpleDateFormat() = " << status;
#else
        mFormat = static_cast<Qt::DateFormat>(-1); // custom
#endif

        QString format = mCustomFormat;
        format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
#ifdef ICU_VERSION
        if (format.contains(QString("SSS")))
            timerInterval = 1;
        else if (format.contains(QString("SS")))
            timerInterval = 10;
        else if (format.contains(QString("S")))
            timerInterval = 100;
#else
        if (format.contains(QString("z")))
            timerInterval = 1;
#endif
        else if (format.contains(QString("s")))
            timerInterval = 1000;
        else
            timerInterval = 60000;
    }
        break;

    case FORMAT_FULL:
#ifdef ICU_VERSION
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kFull, DateFormat::kFull, *mLocale);
#else
        mFormat = Qt::RFC2822Date;
#endif
        timerInterval = 1000;
        break;

    case FORMAT_LONG:
#ifdef ICU_VERSION
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kLong, DateFormat::kLong, *mLocale);
#else
        mFormat = Qt::DefaultLocaleLongDate;
#endif
        timerInterval = 1000;
        break;

    case FORMAT_MEDIUM:
#ifdef ICU_VERSION
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kMedium, DateFormat::kMedium, *mLocale);
#else
        mFormat = Qt::ISODate;
#endif
        timerInterval = 1000;
        break;

    case FORMAT_SHORT:
#ifdef ICU_VERSION
        mFormat = icu::DateFormat::createDateTimeInstance(DateFormat::kShort, DateFormat::kShort, *mLocale);
#else
        mFormat = Qt::DefaultLocaleShortDate;
#endif
        timerInterval = 60000;
        break;

    default:;
    }

    restartTimer(timerInterval);

#ifdef ICU_VERSION
    if (mCalendar)
        mFormat->setCalendar(*mCalendar);
#endif
}

void LxQtWorldClock::restartTimer(int timerInterval)
{
    mTimer->stop();
    mTimer->setInterval(timerInterval);

    if (timerInterval < 1000)
        mTimer->start();
    else
    {
#ifdef ICU_VERSION
        int delay = static_cast<int>((timerInterval + 100 - (static_cast<long long>(Calendar::getNow()) % timerInterval)) % timerInterval);
#else
        int delay = static_cast<int>((timerInterval + 100 - (static_cast<long long>(QTime::currentTime().msecsSinceStartOfDay()) % timerInterval)) % timerInterval);
#endif
        QTimer::singleShot(delay, this, SLOT(timeout()));
        QTimer::singleShot(delay, mTimer, SLOT(start()));
    }
}

#ifdef ICU_VERSION
void LxQtWorldClock::updateTimezone()
{
    if (mFormat)
    {
        if (mCalendar)
            delete mCalendar;

        UErrorCode status = U_ZERO_ERROR;

#if ICU_VERSION >= 48
        char region[3];
        icu::TimeZone::getRegion(mActiveTimeZone.toLatin1().data(), region, sizeof(region) / sizeof(char), status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone: TimeZone::getRegion() status = " << status;

        if (mLocale)
            delete mLocale;
        mLocale = new icu::Locale(mDefaultLanguage.toLatin1().data(), region);
#endif

        icu::UnicodeString timeZoneName = Qt_to_ICU(mActiveTimeZone);
        icu::TimeZone* timeZone = icu::TimeZone::createTimeZone(timeZoneName);
        icu::UnicodeString control;
        if (timeZone->getID(control) != timeZoneName)
            qDebug() << "updateTimezone: TimeZone::createTimeZone(" << mActiveTimeZone << ") returned zone with ID " << ICU_to_Qt(control);

        status = U_ZERO_ERROR;
        mCalendar = icu::Calendar::createInstance(timeZone, *mLocale, status);
        if (U_FAILURE(status))
            qDebug() << "updateTimezone: Calendar::createInstance() status = " << status;
        mFormat->setCalendar(*mCalendar);
    }

    timeout(); // instantly!
}
#endif

void LxQtWorldClock::settingsChanged()
{
    QSettings *_settings = settings();

    FormatType oldFormatType = mFormatType;
    QString oldCustomFormat = mCustomFormat;

    mTimeZones.clear();

    int size = _settings->beginReadArray("timeZones");
    for (int i = 0; i < size; ++i)
    {
        _settings->setArrayIndex(i);
        mTimeZones.append(_settings->value("timeZone", QString()).toString());
    }
    _settings->endArray();
    if (mTimeZones.isEmpty())
#ifdef ICU_VERSION
    {
        icu::TimeZone *timeZone = icu::TimeZone::createDefault();
        icu::UnicodeString timeZoneName;
        timeZone->getID(timeZoneName);
        mTimeZones.append(ICU_to_Qt(timeZoneName));

        delete timeZone;
    }
#else
        mTimeZones.append(QString::fromLatin1(QTimeZone::systemTimeZoneId()));
#endif

    mDefaultTimeZone = _settings->value("defaultTimeZone", QString()).toString();
    if (mDefaultTimeZone.isEmpty())
        mDefaultTimeZone = mTimeZones[0];
    mActiveTimeZone = mDefaultTimeZone;

    mCustomFormat = _settings->value("customFormat", QString("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'eee, d MMM yyyy'<br/>'VVVV'</font>'")).toString();

    QString formatType = _settings->value("formatType", QString()).toString();
    if (formatType == "custom")
        mFormatType = FORMAT_CUSTOM;
    else if (formatType == "full")
        mFormatType = FORMAT_FULL;
    else if (formatType == "long")
        mFormatType = FORMAT_LONG;
    else if (formatType == "medium")
        mFormatType = FORMAT_MEDIUM;
    else
        mFormatType = FORMAT_SHORT;

    if ((oldFormatType != mFormatType) || (oldCustomFormat != mCustomFormat))
        updateFormat();

#ifdef ICU_VERSION
    updateTimezone();
#endif

    bool autoRotate = settings()->value("autoRotate", true).toBool();
    if (autoRotate != mAutoRotate)
    {
        mAutoRotate = autoRotate;
        realign();
    }

    if (mPopup)
    {
        updatePopupContent();
        mPopup->adjustSize();
        mPopup->setGeometry(calculatePopupWindowPos(mPopup->size()));
    }

    timeout();
}

QDialog *LxQtWorldClock::configureDialog()
{
    return new LxQtWorldClockConfiguration(settings());
}

void LxQtWorldClock::wheelScrolled(int delta)
{
    if (mTimeZones.count() > 1)
    {
        mActiveTimeZone = mTimeZones[(mTimeZones.indexOf(mActiveTimeZone) + ((delta > 0) ? -1 : 1) + mTimeZones.size()) % mTimeZones.size()];

#ifdef ICU_VERSION
        updateTimezone();
#else
        timeout();
#endif
    }
}

void LxQtWorldClock::activated(ActivationReason reason)
{
    switch (reason)
    {
    case ILxQtPanelPlugin::Trigger:
    case ILxQtPanelPlugin::MiddleClick:
        break;

    default:
        return;
    }

    if (!mPopup)
    {
#ifdef ICU_VERSION
        UErrorCode status = U_ZERO_ERROR;
#endif

        mPopup = new QDialog(mContent);
        mPopup->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::X11BypassWindowManagerHint);
        mPopup->setLayout(new QHBoxLayout(mPopup));

        if (reason == ILxQtPanelPlugin::Trigger)
        {
            mPopup->setObjectName(QString());

            mPopup->layout()->setContentsMargins(0, 0, 0, 0);
            QCalendarWidget *calendarWidget = new QCalendarWidget(mPopup);
            mPopup->layout()->addWidget(calendarWidget);

#ifdef ICU_VERSION
            status = U_ZERO_ERROR;
            UCalendarDaysOfWeek first = mCalendar->getFirstDayOfWeek(status);
            if (U_FAILURE(status))
                qDebug() << "popupDialog:calendar: Calendar::getFirstDayOfWeek() status = " << status;
            calendarWidget->setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(((static_cast<int>(first) + 5) % 7) + 1));

            status = U_ZERO_ERROR;
            calendarWidget->setSelectedDate(QDate(mCalendar->get(UCAL_YEAR, status), mCalendar->get(UCAL_MONTH, status) - UCAL_JANUARY + 1, mCalendar->get(UCAL_DATE, status)));
            if (U_FAILURE(status))
                qDebug() << "popupDialog:calendar: Calendar:get() status = " << status;
#else
            QTimeZone timeZone(mActiveTimeZone.toLatin1());
            calendarWidget->setFirstDayOfWeek(QLocale(QLocale::AnyLanguage, timeZone.country()).firstDayOfWeek());
            calendarWidget->setSelectedDate(QDateTime::currentDateTime().toTimeZone(timeZone).date());
#endif
        }
        else
        {
            mPopup->setObjectName("WorldClockPopup");

            mPopupContent = new QLabel(mPopup);
            mPopup->layout()->addWidget(mPopupContent);
            mPopupContent->setAlignment(mContent->alignment());

            updatePopupContent();
        }

        mPopup->adjustSize();
        mPopup->setGeometry(calculatePopupWindowPos(mPopup->size()));

        mPopup->show();
    }
    else
    {
        mPopupContent = NULL;
        mPopup->deleteLater();
        mPopup = NULL;
    }
}

void LxQtWorldClock::updatePopupContent()
{
    if (mPopupContent)
    {
#ifdef ICU_VERSION
        UErrorCode status = U_ZERO_ERROR;
#else
        QDateTime now = QDateTime::currentDateTime();
#endif
        QStringList allTimeZones;

        foreach (QString qTimeZoneName, mTimeZones)
        {
#ifdef ICU_VERSION
            icu::UnicodeString timeZoneName = Qt_to_ICU(qTimeZoneName);
            icu::TimeZone* timeZone = icu::TimeZone::createTimeZone(timeZoneName);
            icu::UnicodeString control;
            if (timeZone->getID(control) != timeZoneName)
                qDebug() << "popupDialog:time: TimeZone::createTimeZone(" << mActiveTimeZone << ") returned zone with ID " << ICU_to_Qt(control);

            status = U_ZERO_ERROR;
            icu::Calendar *calendar = icu::Calendar::createInstance(timeZone, *mLocale, status);
            if (U_FAILURE(status))
                qDebug() << "popupDialog:time: Calendar::createInstance() status = " << status;
            mFormat->setCalendar(*calendar);

            status = U_ZERO_ERROR;
            UnicodeString str;
            mFormat->format(Calendar::getNow(), str, status);
            if (U_FAILURE(status))
                qDebug() << "popupDialog:time: DateFormat::format() status = " << status;

            delete calendar;

            allTimeZones.append(ICU_to_Qt(str));
#else
            QTimeZone timeZone(qTimeZoneName.toLatin1());
            QDateTime tzNow = now.toTimeZone(timeZone);
            QString str;
            if (mFormat == -1) // custom
                str = tzNow.toString(preformat(mCustomFormat, timeZone, tzNow));
            else
                str = tzNow.toString(mFormat);

            allTimeZones.append(str);
#endif
        }

        mPopupContent->setText(allTimeZones.join("<hr/>"));

#ifdef ICU_VERSION
        //restore current
        mFormat->setCalendar(*mCalendar);
#endif
    }
}

#ifndef ICU_VERSION
QString LxQtWorldClock::preformat(const QString &format, const QTimeZone &timeZone, const QDateTime &dateTime)
{
    QString result = format;
    int from = 0;
    for (;;)
    {
        int apos = result.indexOf(QLatin1Char('\''), from);
        int tz = result.indexOf(QLatin1Char('T'), from);
        if ((apos != -1) && (tz != -1))
        {
            if (apos > tz)
                apos = -1;
            else
                tz = -1;
        }
        if (apos != -1)
        {
            from = apos + 1;
            apos = result.indexOf(QLatin1Char('\''), from);
            if (apos == -1) // misformat
                break;
            from = apos + 1;
        }
        else if (tz != -1)
        {
            int length = 1;
            for (; result[tz + length] == QLatin1Char('T'); ++length);
            if (length > 5)
                length = 5;
            QString replacement;
            switch (length)
            {
            case 1:
                replacement = timeZone.displayName(dateTime, QTimeZone::OffsetName);
                if (replacement.startsWith(QLatin1String("UTC")))
                    replacement = replacement.mid(3);
                break;

            case 2:
                replacement = QString::fromLatin1(timeZone.id());
                break;

            case 3:
                replacement = timeZone.abbreviation(dateTime);
                break;

            case 4:
                replacement = timeZone.displayName(dateTime, QTimeZone::ShortName);
                break;

            case 5:
                replacement = timeZone.displayName(dateTime, QTimeZone::LongName);
                break;
            }

            if ((tz > 0) && (result[tz - 1] == QLatin1Char('\'')))
            {
                --tz;
                ++length;
            }
            else
                replacement.prepend(QLatin1Char('\''));

            if (result[tz + length] == QLatin1Char('\''))
                ++length;
            else
                replacement.append(QLatin1Char('\''));

            result.replace(tz, length, replacement);
            from = tz + replacement.length();
        }
        else
            break;
    }
    return result;
}
#endif

void LxQtWorldClock::realign()
{
    if (mAutoRotate)
        switch (panel()->position())
        {
        case ILxQtPanel::PositionTop:
        case ILxQtPanel::PositionBottom:
            mRotatedWidget->setOrigin(Qt::TopLeftCorner);
            break;

        case ILxQtPanel::PositionLeft:
            mRotatedWidget->setOrigin(Qt::BottomLeftCorner);
            break;

        case ILxQtPanel::PositionRight:
            mRotatedWidget->setOrigin(Qt::TopRightCorner);
            break;
        }
    else
        mRotatedWidget->setOrigin(Qt::TopLeftCorner);
}

ActiveLabel::ActiveLabel(QWidget * parent) :
    QLabel(parent)
{
}

void ActiveLabel::wheelEvent(QWheelEvent *event)
{
    emit wheelScrolled(event->delta());

    QLabel::wheelEvent(event);
}

void ActiveLabel::mouseReleaseEvent(QMouseEvent* event)
{
    switch (event->button())
    {
    case Qt::LeftButton:
        emit leftMouseButtonClicked();
        break;

    case Qt::MidButton:
        emit middleMouseButtonClicked();
        break;

    default:;
    }

    QLabel::mouseReleaseEvent(event);
}
