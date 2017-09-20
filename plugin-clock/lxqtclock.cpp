/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2013 Razor team
 * Authors:
 *   Christopher "VdoP" Regali
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include "lxqtclock.h"

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

#include <QDateTime>
#include <QTimer>
#include <QPoint>
#include <QRect>
#include <QProxyStyle>
#include <QPainter>

#include <QDebug>
#include <LXQt/Notification>

static void notifyAboutDeprecation(PluginSettings * settings)
{
    // this is called only from LXQtClock c-tor...so just from main thread, no locking needed
    // Note: we get/store the "dont_show" in section of particular instance of clock => this is not 100% valid
    // for al possible changes of panel configuration, but we can live with this
    static bool notification_done = settings->value(QLatin1String("dont_show_deprecation"), false).toBool();
    if (!notification_done)
    {
        LXQt::Notification * notification = new LXQt::Notification{LXQtClock::tr("Date&Time (clock) plugin is deprecated")};
        notification->setBody(LXQtClock::tr("The <strong>clock</strong> plugin is deprecated and will be removed in future version of LXQt. Consider"
                    " replacing it with <strong>worldclock</strong>.<br/>"));
        notification->setActions({LXQtClock::tr("don't show this again")});
        notification->setTimeout(5000);
        QObject::connect(notification, &LXQt::Notification::actionActivated, settings, [notification, settings] (int actionNumber) -> void
                {
                    if (actionNumber == 0)
                        settings->setValue(QLatin1String("dont_show_deprecation"), true);
                    notification->close();
                });
        QObject::connect(notification, &LXQt::Notification::notificationClosed, notification, &QObject::deleteLater);
        notification->update();
        notification_done = true;
    }
}

/**
 * @file lxqtclock.cpp
 * @brief implements LXQtclock and LXQtclockgui
 * @author Christopher "VdoP" Regali
 * @author Kuzma Shapran
 */

class DownscaleFontStyle : public QProxyStyle
{
    using QProxyStyle::QProxyStyle;
public:

    virtual void drawItemText(QPainter * painter, const QRect & rect, int flags
            , const QPalette & pal, bool enabled, const QString & text
            , QPalette::ColorRole textRole = QPalette::NoRole) const override
    {
        while (1 < painter->font().pointSize()
                && !(rect.size() - painter->fontMetrics().boundingRect(text).size()).isValid())
        {
            QFont f{painter->font()};
            f.setPointSize(f.pointSize() - 1);
            painter->setFont(f);
        }
        return QProxyStyle::drawItemText(painter, rect, flags, pal, enabled, text, textRole);
    }
};

/**
 * @brief constructor
 */
LXQtClock::LXQtClock(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mAutoRotate(true),
    mTextStyle{new DownscaleFontStyle},
    mCurrentCharCount(0)
{
    QTimer::singleShot(0, this, [this] { notifyAboutDeprecation(settings()); });

    mMainWidget = new QWidget();
    mRotatedWidget = new LXQt::RotatedWidget(*(new QWidget()), mMainWidget);
    mContent = mRotatedWidget->content();
    mContent->setStyle(mTextStyle.data());
    mTimeLabel = new QLabel(mContent);
    mDateLabel = new QLabel(mContent);

    QVBoxLayout *borderLayout = new QVBoxLayout(mMainWidget);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->addWidget(mRotatedWidget, 0, Qt::AlignCenter);

    mTimeLabel->setObjectName("TimeLabel");
    mDateLabel->setObjectName("DateLabel");

    mTimeLabel->setAlignment(Qt::AlignCenter);
    mDateLabel->setAlignment(Qt::AlignCenter);

    mContent->setLayout(new QVBoxLayout{mContent});
    mContent->layout()->setContentsMargins(0, 0, 0, 0);
    mContent->layout()->setSpacing(0);
    mContent->layout()->addWidget(mTimeLabel);
    mContent->layout()->addWidget(mDateLabel);

    mClockTimer = new QTimer(this);
    mClockTimer->setTimerType(Qt::PreciseTimer);
    connect (mClockTimer, SIGNAL(timeout()), SLOT(updateTime()));

    mClockFormat = "hh:mm";

    mCalendarPopup = new CalendarPopup(mContent);

    mMainWidget->installEventFilter(this);
    settingsChanged();
}

/**
 * @brief destructor
 */
LXQtClock::~LXQtClock()
{
    delete mMainWidget;
}

QDateTime LXQtClock::currentDateTime()
{
    return QDateTime(mUseUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void LXQtClock::updateTime()
{
    //XXX: do we need this with PreciseTimer ?
    if (currentDateTime().time().msec() > 500)
        restartTimer();

    showTime();
}

void LXQtClock::showTime()
{
    QDateTime now{currentDateTime()};
    int new_char_count;
    if (mDateOnNewLine)
    {
        QString new_time = QLocale::system().toString(now, mTimeFormat);
        QString new_date = QLocale::system().toString(now, mDateFormat);
        new_char_count = qMax(new_time.size(), new_date.size());
        mTimeLabel->setText(new_time);
        mDateLabel->setText(new_date);
    }
    else
    {
        QString new_time = QLocale::system().toString(now, mClockFormat);
        new_char_count = new_time.size();
        mTimeLabel->setText(new_time);
    }

    //Note: if transformation (custom rendering) is enabled we need the explicit update
    //(update doesn't cause superfluous paint events)
    mRotatedWidget->update();

    if (mCurrentCharCount != new_char_count)
    {
        mCurrentCharCount = new_char_count;
        realign();
    }
}

void LXQtClock::restartTimer()
{
    if (mClockTimer->isActive())
        mClockTimer->stop();
    int updateInterval = mClockTimer->interval();
    QDateTime now{currentDateTime()};
    int delay = updateInterval - ((now.time().msec() + now.time().second() * 1000) % updateInterval);
    QTimer::singleShot(delay, Qt::PreciseTimer, mClockTimer, SLOT(start()));
    QTimer::singleShot(delay, Qt::PreciseTimer, this, SLOT(updateTime()));
}

void LXQtClock::settingsChanged()
{
    mFirstDayOfWeek = settings()->value("firstDayOfWeek", -1).toInt();
    if (-1 == mFirstDayOfWeek)
        mCalendarPopup->setFirstDayOfWeek(QLocale::system().firstDayOfWeek());
    else
        mCalendarPopup->setFirstDayOfWeek(static_cast<Qt::DayOfWeek>(mFirstDayOfWeek));

    mTimeFormat = settings()->value("timeFormat", QLocale::system().timeFormat(QLocale::ShortFormat).toUpper().contains("AP") ? "h:mm AP" : "HH:mm").toString();

    mUseUTC = settings()->value("UTC", false).toBool();
    if (mUseUTC)
        mTimeFormat += "' Z'";

    mDateFormat = settings()->value("dateFormat", Qt::SystemLocaleShortDate).toString();

    bool dateBeforeTime = (settings()->value("showDate", "no").toString().toLower() == "before");
    bool dateAfterTime = (settings()->value("showDate", "no").toString().toLower() == "after");
    mDateOnNewLine = (settings()->value("showDate", "no").toString().toLower() == "below");

    mAutoRotate = settings()->value("autoRotate", true).toBool();

    if (dateBeforeTime)
        mClockFormat = QString("%1 %2").arg(mDateFormat).arg(mTimeFormat);
    else if (dateAfterTime)
        mClockFormat = QString("%1 %2").arg(mTimeFormat).arg(mDateFormat);
    else
        mClockFormat = mTimeFormat;

    mDateLabel->setHidden(!mDateOnNewLine);

    // mDateFormat usually does not contain time portion, but since it's possible to use custom date format - it has to be supported. [Kuzma Shapran]
    int updateInterval = QString(mTimeFormat + " " + mDateFormat).replace(QRegExp("'[^']*'"),"").contains("s") ? 1000 : 60000;

    QDateTime now = currentDateTime();

    showTime();

    if (mClockTimer->interval() != updateInterval)
    {
        mClockTimer->setInterval(updateInterval);

        restartTimer();
    }
}

void LXQtClock::realign()
{
    QSize size{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};
    Qt::Corner origin = Qt::TopLeftCorner;
    if (mAutoRotate || panel()->isHorizontal())
    {
        switch (panel()->position())
        {
        case ILXQtPanel::PositionTop:
        case ILXQtPanel::PositionBottom:
            origin = Qt::TopLeftCorner;
            break;

        case ILXQtPanel::PositionLeft:
            origin = Qt::BottomLeftCorner;
            break;

        case ILXQtPanel::PositionRight:
            origin = Qt::TopRightCorner;
            break;
        }

        //set minwidth
        QFontMetrics metrics{mTimeLabel->font()};
        //Note: using a constant string of reasonably wide characters for computing the width
        //      (not the current text as width of text can differ for each particular string (based on font))
        size.setWidth(metrics.boundingRect(QString{mCurrentCharCount, 'A'}).width());
    } else if (!panel()->isHorizontal())
    {
        size.setWidth(panel()->globalGometry().width());
    }

    mTimeLabel->setFixedWidth(size.width());
    mDateLabel->setFixedWidth(size.width());

    int label_height = mTimeLabel->sizeHint().height();
    size.setHeight(mDateOnNewLine ? label_height * 2 : label_height);

    const bool changed = mContent->maximumSize() != size || mRotatedWidget->origin() != origin;

    mContent->setFixedSize(size);
    mRotatedWidget->setOrigin(origin);

    if (changed)
    {
        mRotatedWidget->adjustContentSize();
        mRotatedWidget->update();
    }
}

void LXQtClock::activated(ActivationReason reason)
{
    if (reason != ILXQtPanelPlugin::Trigger)
        return;

    if (!mCalendarPopup->isVisible())
    {
        QRect pos = calculatePopupWindowPos(mCalendarPopup->size());
        mCalendarPopup->move(pos.topLeft());
        willShowWindow(mCalendarPopup);
        mCalendarPopup->show();
    }
    else
    {
        mCalendarPopup->hide();
    }
}

QDialog * LXQtClock::configureDialog()
{
     return new LXQtClockConfiguration(settings());
}

bool LXQtClock::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mMainWidget)
    {
        if (event->type() == QEvent::ToolTip)
            mMainWidget->setToolTip(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));

        return false;
    }

    return false;
}
