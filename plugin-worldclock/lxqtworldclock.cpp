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

#include <QCalendarWidget>
#include <QDate>
#include <QDesktopWidget>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QLocale>
#include <QScopedArrayPointer>
#include <QTimer>
#include <QWheelEvent>


LxQtWorldClock::LxQtWorldClock(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mPopup(NULL),
    mTimer(new QTimer(this)),
    mAutoRotate(true),
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

    mContent->setObjectName(QLatin1String("WorldClockContent"));

    mContent->setAlignment(Qt::AlignCenter);

    settingsChanged();

    connect(mTimer, SIGNAL(timeout()), SLOT(timeout()));

    connect(mContent, SIGNAL(wheelScrolled(int)), SLOT(wheelScrolled(int)));
}

LxQtWorldClock::~LxQtWorldClock()
{
    delete mMainWidget;
}

void LxQtWorldClock::timeout()
{
    QString str = formatDateTime(QDateTime::currentDateTime(), mActiveTimeZone);
    if (str != mLastShownText)
    {
        mContent->setText(str);
        mLastShownText = str;

        mRotatedWidget->adjustContentSize();
        mRotatedWidget->update();

        updatePopupContent();
    }
}


void LxQtWorldClock::restartTimer(int timerInterval)
{
    mTimer->stop();
    mTimer->setInterval(timerInterval);

    if (timerInterval < 1000)
        mTimer->start();
    else
    {
        int delay = static_cast<int>((timerInterval + 100 - (static_cast<long long>(QTime::currentTime().msecsSinceStartOfDay()) % timerInterval)) % timerInterval);
        QTimer::singleShot(delay, this, SLOT(timeout()));
        QTimer::singleShot(delay, mTimer, SLOT(start()));
    }
}

void LxQtWorldClock::settingsChanged()
{
    QSettings *_settings = settings();

    QString oldFormat = mFormat;

    mTimeZones.clear();

    int size = _settings->beginReadArray(QLatin1String("timeZones"));
    for (int i = 0; i < size; ++i)
    {
        _settings->setArrayIndex(i);
        QString timeZoneName = _settings->value(QLatin1String("timeZone"), QString()).toString();
        mTimeZones.append(timeZoneName);
        mTimeZoneCustomNames[timeZoneName] = _settings->value(QLatin1String("customName"), QString()).toString();
    }
    _settings->endArray();
    if (mTimeZones.isEmpty())
        mTimeZones.append(QString::fromLatin1(QTimeZone::systemTimeZoneId()));

    mDefaultTimeZone = _settings->value(QLatin1String("defaultTimeZone"), QString()).toString();
    if (mDefaultTimeZone.isEmpty())
        mDefaultTimeZone = mTimeZones[0];
    mActiveTimeZone = mDefaultTimeZone;


    bool longTimeFormatSelected = false;

    QString formatType = _settings->value(QLatin1String("formatType"), QString()).toString();
    QString dateFormatType = _settings->value(QLatin1String("dateFormatType"), QString()).toString();
    bool advancedManual = _settings->value(QLatin1String("useAdvancedManualFormat"), false).toBool();

    // backward compatibility
    if (formatType == QLatin1String("custom"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = true;
    }
    else if (formatType == QLatin1String("short"))
    {
        formatType = QLatin1String("short-timeonly");
        dateFormatType = QLatin1String("short");
        advancedManual = false;
    }
    else if ((formatType == QLatin1String("full")) ||
             (formatType == QLatin1String("long")) ||
             (formatType == QLatin1String("medium")))
    {
        formatType = QLatin1String("long-timeonly");
        dateFormatType = QLatin1String("long");
        advancedManual = false;
    }

    if (formatType == QLatin1String("long-timeonly"))
        longTimeFormatSelected = true;

    bool timeShowSeconds = _settings->value(QLatin1String("timeShowSeconds"), false).toBool();
    bool timePadHour = _settings->value(QLatin1String("timePadHour"), false).toBool();
    bool timeAMPM = _settings->value(QLatin1String("timeAMPM"), false).toBool();

    // timezone
    bool showTimezone = _settings->value(QLatin1String("showTimezone"), false).toBool() && !longTimeFormatSelected;

    QString timezonePosition = _settings->value(QLatin1String("timezonePosition"), QString()).toString();
    QString timezoneFormatType = _settings->value(QLatin1String("timezoneFormatType"), QString()).toString();

    // date
    bool showDate = _settings->value(QLatin1String("showDate"), false).toBool();

    QString datePosition = _settings->value(QLatin1String("datePosition"), QString()).toString();

    bool dateShowYear = _settings->value(QLatin1String("dateShowYear"), false).toBool();
    bool dateShowDoW = _settings->value(QLatin1String("dateShowDoW"), false).toBool();
    bool datePadDay = _settings->value(QLatin1String("datePadDay"), false).toBool();
    bool dateLongNames = _settings->value(QLatin1String("dateLongNames"), false).toBool();

    // advanced
    QString customFormat = _settings->value(QLatin1String("customFormat"), tr("'<b>'HH:mm:ss'</b><br/><font size=\"-2\">'ddd, d MMM yyyy'<br/>'TT'</font>'")).toString();

    if (advancedManual)
        mFormat = customFormat;
    else
    {
        QTimeZone timeZone(mActiveTimeZone.toLatin1());
        QLocale locale = QLocale(QLocale::AnyLanguage, QLocale().country());

        if (formatType == QLatin1String("short-timeonly"))
            mFormat = locale.timeFormat(QLocale::ShortFormat);
        else if (formatType == QLatin1String("long-timeonly"))
            mFormat = locale.timeFormat(QLocale::LongFormat);
        else // if (formatType == QLatin1String("custom-timeonly"))
            mFormat = QString(QLatin1String("%1:mm%2%3")).arg(timePadHour ? QLatin1String("hh") : QLatin1String("h")).arg(timeShowSeconds ? QLatin1String(":ss") : QLatin1String("")).arg(timeAMPM ? QLatin1String(" A") : QLatin1String(""));

        if (showTimezone)
        {
            QString timezonePortion;
            if (timezoneFormatType == QLatin1String("short"))
                timezonePortion = QLatin1String("TTTT");
            else if (timezoneFormatType == QLatin1String("long"))
                timezonePortion = QLatin1String("TTTTT");
            else if (timezoneFormatType == QLatin1String("offset"))
                timezonePortion = QLatin1String("T");
            else if (timezoneFormatType == QLatin1String("abbreviation"))
                timezonePortion = QLatin1String("TTT");
            else if (timezoneFormatType == QLatin1String("iana"))
                timezonePortion = QLatin1String("TT");
            else // if (timezoneFormatType == QLatin1String("custom"))
                timezonePortion = QLatin1String("TTTTTT");

            if (timezonePosition == QLatin1String("below"))
                mFormat = mFormat + QLatin1String("'<br/>'") + timezonePortion;
            else if (timezonePosition == QLatin1String("above"))
                mFormat = timezonePortion + QLatin1String("'<br/>'") + mFormat;
            else if (timezonePosition == QLatin1String("before"))
                mFormat = timezonePortion + QLatin1String(" ") + mFormat;
            else // if (timezonePosition == QLatin1String("after"))
                mFormat = mFormat + QLatin1String(" ") + timezonePortion;
        }

        if (showDate)
        {
            QString datePortion;
            if (dateFormatType == QLatin1String("short"))
                datePortion = locale.dateFormat(QLocale::ShortFormat);
            else if (dateFormatType == QLatin1String("long"))
                datePortion = locale.dateFormat(QLocale::LongFormat);
            else if (dateFormatType == QLatin1String("iso"))
                datePortion = QLatin1String("yyyy-MM-dd");
            else // if (dateFormatType == QLatin1String("custom"))
            {
                QString datePortionOrder;
                QString dateLocale = locale.dateFormat(QLocale::ShortFormat).toLower();
                int yearIndex = dateLocale.indexOf("y");
                int monthIndex = dateLocale.indexOf("m");
                int dayIndex = dateLocale.indexOf("d");
                if (yearIndex < dayIndex)
                // Big-endian (year, month, day) (yyyy MMMM dd, dddd) -> in some Asia countires like China or Japan
                    datePortionOrder = QLatin1String("%1%2%3 %4%5%6");
                else if (monthIndex < dayIndex)
                // Middle-endian (month, day, year) (dddd, MMMM dd yyyy) -> USA
                    datePortionOrder = QLatin1String("%6%5%3 %4%2%1");
                else
                // Little-endian (day, month, year) (dddd, dd MMMM yyyy) -> most of Europe
                    datePortionOrder = QLatin1String("%6%5%4 %3%2%1");
                datePortion = datePortionOrder.arg(dateShowYear ? QLatin1String("yyyy") : QLatin1String("")).arg(dateShowYear ? QLatin1String(" ") : QLatin1String("")).arg(dateLongNames ? QLatin1String("MMMM") : QLatin1String("MMM")).arg(datePadDay ? QLatin1String("dd") : QLatin1String("d")).arg(dateShowDoW ? QLatin1String(", ") : QLatin1String("")).arg(dateShowDoW ? (dateLongNames ? QLatin1String("dddd") : QLatin1String("ddd")) : QLatin1String(""));
            }

            if (datePosition == QLatin1String("below"))
                mFormat = mFormat + QLatin1String("'<br/>'") + datePortion;
            else if (datePosition == QLatin1String("above"))
                mFormat = datePortion + QLatin1String("'<br/>'") + mFormat;
            else if (datePosition == QLatin1String("before"))
                mFormat = datePortion + QLatin1String(" ") + mFormat;
            else // if (datePosition == QLatin1String("after"))
                mFormat = mFormat + QLatin1String(" ") + datePortion;
        }
    }


    if ((oldFormat != mFormat))
    {
        int timerInterval = 0;

        QString format = mFormat;
        format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
        if (format.contains(QLatin1String("z")))
            timerInterval = 1;
        else if (format.contains(QLatin1String("s")))
            timerInterval = 1000;
        else
            timerInterval = 60000;

        restartTimer(timerInterval);
    }

    bool autoRotate = settings()->value(QLatin1String("autoRotate"), true).toBool();
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
        timeout();
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
        mPopup = new LxQtWorldClockPopup(mContent);
        connect(mPopup, SIGNAL(deactivated()), SLOT(deletePopup()));

        if (reason == ILxQtPanelPlugin::Trigger)
        {
            mPopup->setObjectName(QLatin1String("WorldClockCalendar"));

            mPopup->layout()->setContentsMargins(0, 0, 0, 0);
            QCalendarWidget *calendarWidget = new QCalendarWidget(mPopup);
            mPopup->layout()->addWidget(calendarWidget);

            QTimeZone timeZone(mActiveTimeZone.toLatin1());
            calendarWidget->setFirstDayOfWeek(QLocale(QLocale::AnyLanguage, timeZone.country()).firstDayOfWeek());
            calendarWidget->setSelectedDate(QDateTime::currentDateTime().toTimeZone(timeZone).date());
        }
        else
        {
            mPopup->setObjectName(QLatin1String("WorldClockPopup"));

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
        deletePopup();
    }
}

void LxQtWorldClock::deletePopup()
{
    mPopupContent = NULL;
    mPopup->deleteLater();
    mPopup = NULL;
}

QString LxQtWorldClock::formatDateTime(const QDateTime &datetime, const QString &timeZoneName)
{
    QTimeZone timeZone(timeZoneName.toLatin1());
    QDateTime tzNow = datetime.toTimeZone(timeZone);
    return tzNow.toString(preformat(mFormat, timeZone, tzNow));
}

void LxQtWorldClock::updatePopupContent()
{
    if (mPopupContent)
    {
        QDateTime now = QDateTime::currentDateTime();
        QStringList allTimeZones;
        bool hasTimeZone = formatHasTimeZone(mFormat);

        foreach (QString timeZoneName, mTimeZones)
        {
            QString formatted = formatDateTime(now, timeZoneName);

            if (!hasTimeZone)
                formatted += QLatin1String("<br/>") + QString::fromLatin1(QTimeZone(timeZoneName.toLatin1()).id());

            allTimeZones.append(formatted);
        }

        mPopupContent->setText(allTimeZones.join(QLatin1String("<hr/>")));
    }
}

bool LxQtWorldClock::formatHasTimeZone(QString format)
{
    format.replace(QRegExp(QLatin1String("'[^']*'")), QString());
    return format.toLower().contains(QLatin1String("t"));
}

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
            if (length > 6)
                length = 6;
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

            case 6:
                replacement = mTimeZoneCustomNames[QString::fromLatin1(timeZone.id())];
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

ActiveLabel::ActiveLabel(QWidget *parent) :
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

LxQtWorldClockPopup::LxQtWorldClockPopup(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint | Qt::Popup | Qt::X11BypassWindowManagerHint)
{
    setLayout(new QHBoxLayout(this));
    layout()->setMargin(1);
}

void LxQtWorldClockPopup::show()
{
    QDialog::show();
    activateWindow();
}

bool LxQtWorldClockPopup::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate)
        emit deactivated();

    return QDialog::event(event);
}
