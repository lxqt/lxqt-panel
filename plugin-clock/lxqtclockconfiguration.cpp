/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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


#include <QInputDialog>
#include <QStandardItemModel>
#include <QStandardItem>

#include "lxqtclockconfiguration.h"
#include "ui_lxqtclockconfiguration.h"

namespace
{
    class FirstDayCombo : public QStandardItemModel
    {
    public:
        FirstDayCombo()
        {
            QStandardItem* item = 0;
            int row = 0;
            item = new QStandardItem;
            item->setData(-1, Qt::UserRole);
            setItem(row, 0, item);
            item = new QStandardItem;
            item->setData(tr("<locale based>"), Qt::DisplayRole);
            setItem(row, 1, item);
            ++row;
            for (int wday = Qt::Monday; Qt::Sunday >= wday; ++wday, ++row)
            {
                item = new QStandardItem;
                item->setData(wday, Qt::UserRole);
                setItem(row, 0, item);
                item = new QStandardItem;
                item->setData(QLocale::system().dayName(wday), Qt::DisplayRole);
                setItem(row, 1, item);
            }
        }

        int findIndex(int wday)
        {
            int i = rowCount() - 1;
            for ( ; 0 <= i; --i)
            {
                if (item(i)->data(Qt::UserRole).toInt() == wday)
                    break;
            }
            return i;
        }

    };
}

LXQtClockConfiguration::LXQtClockConfiguration(PluginSettings *settings, QWidget *parent) :
    LXQtPanelPluginConfigDialog(settings, parent),
    ui(new Ui::LXQtClockConfiguration),
    mOldIndex(1)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("ClockConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), SLOT(dialogButtonsAction(QAbstractButton*)));

    ui->firstDayOfWeekCB->setModel(new FirstDayCombo);
    ui->firstDayOfWeekCB->setModelColumn(1);

    loadSettings();
    /* We use clicked() and activated(int) because these signals aren't emitting after programmaticaly
      change of state */

    connect(ui->dateFormatCOB, SIGNAL(activated(int)), SLOT(dateFormatActivated(int)));

    connect(ui->showSecondsCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->ampmClockCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->useUtcCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->dontShowDateRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateBeforeTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateAfterTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->showDateBelowTimeRB, SIGNAL(clicked()), SLOT(saveSettings()));

    connect(ui->autorotateCB, SIGNAL(clicked()), SLOT(saveSettings()));
    connect(ui->firstDayOfWeekCB, SIGNAL(activated(int)), SLOT(saveSettings()));
}

LXQtClockConfiguration::~LXQtClockConfiguration()
{
    delete ui;
}

static int currentYear = QDate::currentDate().year();

void LXQtClockConfiguration::addDateFormat(const QString &format)
{
    if (ui->dateFormatCOB->findData(QVariant(format)) == -1)
        ui->dateFormatCOB->addItem(QDate(currentYear, 1, 1).toString(format), QVariant(format));
}

void LXQtClockConfiguration::createDateFormats()
{
    ui->dateFormatCOB->clear();

    QString systemDateLocale = QLocale::system().dateFormat(QLocale::ShortFormat).toUpper();

    if (systemDateLocale.indexOf("Y") < systemDateLocale.indexOf("D"))
    // Big-endian (year, month, day) -> in some Asia countires like China or Japan
    {
        addDateFormat("MMM d");
        addDateFormat("MMMM d");
        addDateFormat("MMM d, ddd");
        addDateFormat("MMMM d, dddd");
        addDateFormat("yyyy MMM d");
        addDateFormat("yyyy MMMM d");
        addDateFormat("yyyy MMM d, ddd");
        addDateFormat("yyyy MMMM d, dddd");
        addDateFormat("MMM dd");
        addDateFormat("MMMM dd");
        addDateFormat("MMM dd, ddd");
        addDateFormat("MMMM dd, dddd");
        addDateFormat("yyyy MMM dd");
        addDateFormat("yyyy MMMM dd");
        addDateFormat("yyyy MMM dd, ddd");
        addDateFormat("yyyy MMMM dd, dddd");
    }
    else if (systemDateLocale.indexOf("M") < systemDateLocale.indexOf("D"))
    // Middle-endian (month, day, year) -> USA
    {
        addDateFormat("MMM d");
        addDateFormat("MMMM d");
        addDateFormat("ddd, MMM d");
        addDateFormat("dddd, MMMM d");
        addDateFormat("MMM d yyyy");
        addDateFormat("MMMM d yyyy");
        addDateFormat("ddd, MMM d yyyy");
        addDateFormat("dddd, MMMM d yyyy");
        addDateFormat("MMM dd");
        addDateFormat("MMMM dd");
        addDateFormat("ddd, MMM dd");
        addDateFormat("dddd, MMMM dd");
        addDateFormat("MMM dd yyyy");
        addDateFormat("MMMM dd yyyy");
        addDateFormat("ddd, MMM dd yyyy");
        addDateFormat("dddd, MMMM dd yyyy");
    }
    else
    // Little-endian (day, month, year) -> most of Europe
    {
        addDateFormat("d MMM");
        addDateFormat("d MMMM");
        addDateFormat("ddd, d MMM");
        addDateFormat("dddd, d MMMM");
        addDateFormat("d MMM yyyy");
        addDateFormat("d MMMM yyyy");
        addDateFormat("ddd, d MMM yyyy");
        addDateFormat("dddd, d MMMM yyyy");
        addDateFormat("dd MMM");
        addDateFormat("dd MMMM");
        addDateFormat("ddd, dd MMM");
        addDateFormat("dddd, dd MMMM");
        addDateFormat("dd MMM yyyy");
        addDateFormat("dd MMMM yyyy");
        addDateFormat("ddd, dd MMM yyyy");
        addDateFormat("dddd, dd MMMM yyyy");
    }

    addDateFormat(QLocale::system().dateFormat(QLocale::ShortFormat));
    addDateFormat(QLocale::system().dateFormat(QLocale::LongFormat));

    addDateFormat("yyyy-MM-dd"); // ISO

    if (mCustomDateFormat.isEmpty())
        ui->dateFormatCOB->addItem("Custom ...", QVariant(mCustomDateFormat));
    else
        ui->dateFormatCOB->addItem(QString("Custom (%1) ...").arg(QDate(currentYear, 1, 1).toString(mCustomDateFormat)), QVariant(mCustomDateFormat));
}

void LXQtClockConfiguration::loadSettings()
{
    QString systemDateLocale = QLocale::system().dateFormat(QLocale::ShortFormat).toUpper();
    QString systemTimeLocale = QLocale::system().timeFormat(QLocale::ShortFormat).toUpper();

    QString timeFormat = settings().value("timeFormat", systemTimeLocale.contains("AP") ? "h:mm AP" : "HH:mm").toString();

    ui->showSecondsCB->setChecked(timeFormat.indexOf("ss") > -1);

    ui->ampmClockCB->setChecked(timeFormat.toUpper().indexOf("AP") > -1);

    ui->useUtcCB->setChecked(settings().value("UTC", false).toBool());

    ui->dontShowDateRB->setChecked(true);
    ui->showDateBeforeTimeRB->setChecked(settings().value("showDate", "no").toString().toLower() == "before");
    ui->showDateAfterTimeRB->setChecked(settings().value("showDate", "no").toString().toLower() == "after");
    ui->showDateBelowTimeRB->setChecked(settings().value("showDate", "no").toString().toLower() == "below");

    mCustomDateFormat = settings().value("customDateFormat", QString()).toString();
    QString dateFormat = settings().value("dateFormat", QLocale::system().dateFormat(QLocale::ShortFormat)).toString();

    createDateFormats();

    if (mCustomDateFormat == dateFormat)
        ui->dateFormatCOB->setCurrentIndex(ui->dateFormatCOB->count() - 1);
    else
    {
        ui->dateFormatCOB->setCurrentIndex(ui->dateFormatCOB->findData(dateFormat));
        if (ui->dateFormatCOB->currentIndex() < 0)
            ui->dateFormatCOB->setCurrentIndex(1);
    }
    mOldIndex = ui->dateFormatCOB->currentIndex();

    ui->autorotateCB->setChecked(settings().value("autoRotate", true).toBool());
    ui->firstDayOfWeekCB->setCurrentIndex(dynamic_cast<FirstDayCombo&>(*(ui->firstDayOfWeekCB->model())).findIndex(settings().value("firstDayOfWeek", -1).toInt()));
}

void LXQtClockConfiguration::saveSettings()
{
    QString timeFormat(ui->ampmClockCB->isChecked() ? "h:mm AP" : "HH:mm");

    if (ui->showSecondsCB->isChecked())
        timeFormat.insert(timeFormat.indexOf("mm") + 2, ":ss");

    settings().setValue("timeFormat", timeFormat);

    settings().setValue("UTC", ui->useUtcCB->isChecked());

    settings().setValue("showDate",
        ui->showDateBeforeTimeRB->isChecked() ? "before" :
        (ui->showDateAfterTimeRB->isChecked() ? "after" :
        (ui->showDateBelowTimeRB->isChecked() ? "below" : "no" )));

    settings().setValue("customDateFormat", mCustomDateFormat);
    if (ui->dateFormatCOB->currentIndex() == (ui->dateFormatCOB->count() - 1))
        settings().setValue("dateFormat", mCustomDateFormat);
    else
        settings().setValue("dateFormat", ui->dateFormatCOB->itemData(ui->dateFormatCOB->currentIndex()));

    settings().setValue("autoRotate", ui->autorotateCB->isChecked());
    settings().setValue("firstDayOfWeek", dynamic_cast<QStandardItemModel&>(*ui->firstDayOfWeekCB->model()).item(ui->firstDayOfWeekCB->currentIndex(), 0)->data(Qt::UserRole));
}

void LXQtClockConfiguration::dateFormatActivated(int index)
{
    if (index == ui->dateFormatCOB->count() - 1)
    {
        bool ok;
        QString newCustomDateFormat = QInputDialog::getText(this, tr("Input custom date format"), tr(
            "Interpreted sequences of date format are:\n"
            "\n"
            "d\tthe day as number without a leading zero (1 to 31)\n"
            "dd\tthe day as number with a leading zero (01 to 31)\n"
            "ddd\tthe abbreviated localized day name (e.g. 'Mon' to 'Sun').\n"
            "dddd\tthe long localized day name (e.g. 'Monday' to 'Sunday').\n"
            "M\tthe month as number without a leading zero (1-12)\n"
            "MM\tthe month as number with a leading zero (01-12)\n"
            "MMM\tthe abbreviated localized month name (e.g. 'Jan' to 'Dec').\n"
            "MMMM\tthe long localized month name (e.g. 'January' to 'December').\n"
            "yy\tthe year as two digit number (00-99)\n"
            "yyyy\tthe year as four digit number\n"
            "\n"
            "All other input characters will be treated as text.\n"
            "Any sequence of characters that are enclosed in single quotes (')\n"
            "will also be treated as text and not be used as an expression.\n"
            "\n"
            "\n"
            "Custom date format:"
            ), QLineEdit::Normal, mCustomDateFormat, &ok);
        if (ok)
        {
            mCustomDateFormat = newCustomDateFormat;
            mOldIndex = index;
            createDateFormats();
        }
        ui->dateFormatCOB->setCurrentIndex(mOldIndex);
    }
    else
        mOldIndex = index;

    saveSettings();
}
