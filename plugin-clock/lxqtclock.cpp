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
#include <LXQt/GridLayout>

#include <QDateTime>
#include <QTimer>
#include <QPoint>
#include <QRect>
#include <QProxyStyle>
#include <QPainter>

#include <QDebug>

/**
 * @file lxqtclock.cpp
 * @brief implements LxQtclock and LxQtclockgui
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
LxQtClock::LxQtClock(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mAutoRotate(true),
    mTextStyle{new DownscaleFontStyle}
{
    mMainWidget = new QWidget();
    mRotatedWidget = new LxQt::RotatedWidget(*(new QWidget()), mMainWidget);
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

    mLayout = new LxQt::GridLayout(mContent);
    mLayout->setContentsMargins(0, 0, 0, 0);
    mLayout->setStretch(LxQt::GridLayout::StretchHorizontal | LxQt::GridLayout::StretchVertical);
    mLayout->setColumnCount(1);
    mLayout->addWidget(mTimeLabel);
    mLayout->addWidget(mDateLabel);

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
LxQtClock::~LxQtClock()
{
    delete mMainWidget;
}

QDateTime LxQtClock::currentDateTime()
{
    return QDateTime(mUseUTC ? QDateTime::currentDateTimeUtc() : QDateTime::currentDateTime());
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void LxQtClock::updateTime()
{
    //XXX: do we need this with PreciseTimer ?
    if (currentDateTime().time().msec() > 500)
        restartTimer();

    showTime();
}

void LxQtClock::showTime()
{
    QDateTime now{currentDateTime()};
    if (mDateOnNewLine)
    {
        mTimeLabel->setText(QLocale::system().toString(now, mTimeFormat));
        mDateLabel->setText(QLocale::system().toString(now, mDateFormat));
    }
    else
    {
        mTimeLabel->setText(QLocale::system().toString(now, mClockFormat));
    }
    mTimeLabel->adjustSize();
    mDateLabel->adjustSize();

    mRotatedWidget->update();
}

void LxQtClock::restartTimer()
{
    if (mClockTimer->isActive())
        mClockTimer->stop();
    int updateInterval = mClockTimer->interval();
    QDateTime now{currentDateTime()};
    int delay = updateInterval - ((now.time().msec() + now.time().second() * 1000) % updateInterval);
    QTimer::singleShot(delay, Qt::PreciseTimer, mClockTimer, SLOT(start()));
    QTimer::singleShot(delay, Qt::PreciseTimer, this, SLOT(updateTime()));
}

void LxQtClock::settingsChanged()
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

    realign();

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

void LxQtClock::realign()
{
    QSize min_size{0, 0};
    QSize max_size{QWIDGETSIZE_MAX, QWIDGETSIZE_MAX};
    Qt::Corner origin = Qt::TopLeftCorner;
    if (mAutoRotate)
        switch (panel()->position())
        {
        case ILxQtPanel::PositionTop:
        case ILxQtPanel::PositionBottom:
            origin = Qt::TopLeftCorner;
            break;

        case ILxQtPanel::PositionLeft:
            origin = Qt::BottomLeftCorner;
            break;

        case ILxQtPanel::PositionRight:
            origin = Qt::TopRightCorner;
            break;
        }
    else if (!panel()->isHorizontal())
    {
        min_size.setWidth(panel()->globalGometry().width());
        max_size.setWidth(min_size.width());
    }

    mLayout->setCellMinimumSize(min_size);
    mLayout->setCellMaximumSize(max_size);

    int label_height = mTimeLabel->sizeHint().height();
    min_size.setHeight(mDateOnNewLine ? label_height * 2 : label_height);
    max_size.setHeight(min_size.height());

    const bool changed = mContent->maximumSize() != max_size || mContent->minimumSize() != min_size
        || mRotatedWidget->origin() != origin;

    mContent->setMinimumSize(min_size);
    mContent->setMaximumSize(max_size);
    mRotatedWidget->setOrigin(origin);

    if (changed)
    {
        mRotatedWidget->adjustContentSize();
        mRotatedWidget->update();
    }
}

void LxQtClock::activated(ActivationReason reason)
{
    if (reason != ILxQtPanelPlugin::Trigger)
        return;

    if (!mCalendarPopup->isVisible())
    {
        QRect pos = calculatePopupWindowPos(mCalendarPopup->size());
        mCalendarPopup->move(pos.topLeft());
        mCalendarPopup->show();
    }
    else
    {
        mCalendarPopup->hide();
    }
}

QDialog * LxQtClock::configureDialog()
{
     return new LxQtClockConfiguration(*settings());
}

bool LxQtClock::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mMainWidget)
    {
        if (event->type() == QEvent::ToolTip)
            mMainWidget->setToolTip(QDateTime::currentDateTime().toString(Qt::DefaultLocaleLongDate));

        return false;
    }

    return false;
}
