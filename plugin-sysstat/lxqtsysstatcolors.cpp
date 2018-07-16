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


#include "lxqtsysstatcolors.h"
#include "ui_lxqtsysstatcolors.h"

#include <QSignalMapper>
#include <QColorDialog>


LXQtSysStatColors::LXQtSysStatColors(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LXQtSysStatColors),
    mSelectColorMapper(new QSignalMapper(this))
{
    setWindowModality(Qt::WindowModal);
    ui->setupUi(this);

    mDefaultColors["grid"]  = QColor("#808080");
    mDefaultColors["title"] = QColor("#000000");

    mDefaultColors["cpuSystem"]    = QColor("#800000");
    mDefaultColors["cpuUser"]      = QColor("#000080");
    mDefaultColors["cpuNice"]      = QColor("#008000");
    mDefaultColors["cpuOther"]     = QColor("#808000");
    mDefaultColors["cpuFrequency"] = QColor("#808080");

    mDefaultColors["memApps"]    = QColor("#000080");
    mDefaultColors["memBuffers"] = QColor("#008000");
    mDefaultColors["memCached"]  = QColor("#808000");
    mDefaultColors["memSwap"]    = QColor("#800000");

    mDefaultColors["netReceived"]    = QColor("#000080");
    mDefaultColors["netTransmitted"] = QColor("#808000");


#undef CONNECT_SELECT_COLOUR
#define CONNECT_SELECT_COLOUR(VAR) \
    connect(ui-> VAR ## B, SIGNAL(clicked()), mSelectColorMapper, SLOT(map())); \
    mSelectColorMapper->setMapping(ui-> VAR ## B, QString( #VAR )); \
    mShowColorMap[QString( #VAR )] = ui-> VAR ## B;

    CONNECT_SELECT_COLOUR(grid)
    CONNECT_SELECT_COLOUR(title)
    CONNECT_SELECT_COLOUR(cpuSystem)
    CONNECT_SELECT_COLOUR(cpuUser)
    CONNECT_SELECT_COLOUR(cpuNice)
    CONNECT_SELECT_COLOUR(cpuOther)
    CONNECT_SELECT_COLOUR(cpuFrequency)
    CONNECT_SELECT_COLOUR(memApps)
    CONNECT_SELECT_COLOUR(memBuffers)
    CONNECT_SELECT_COLOUR(memCached)
    CONNECT_SELECT_COLOUR(memSwap)
    CONNECT_SELECT_COLOUR(netReceived)
    CONNECT_SELECT_COLOUR(netTransmitted)

#undef CONNECT_SELECT_COLOUR

    connect(mSelectColorMapper, SIGNAL(mapped(const QString &)), SLOT(selectColor(const QString &)));
}

LXQtSysStatColors::~LXQtSysStatColors()
{
    delete ui;
}

void LXQtSysStatColors::selectColor(const QString &name)
{
    QColor color = QColorDialog::getColor(mColors[name], this);
    if (color.isValid())
    {
        mColors[name] = color;
        mShowColorMap[name]->setStyleSheet(QString("background-color: %1;\ncolor: %2;").arg(color.name()).arg((color.toHsl().lightnessF() > 0.5) ? "black" : "white"));

        ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);
    }
}

void LXQtSysStatColors::setColors(const Colors &colors)
{
    mInitialColors = colors;
    mColors = colors;
    applyColorsToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void LXQtSysStatColors::applyColorsToButtons()
{
    Colors::ConstIterator M = mColors.constEnd();
    for (Colors::ConstIterator I = mColors.constBegin(); I != M; ++I)
    {
        const QColor &color = I.value();
        mShowColorMap[I.key()]->setStyleSheet(QString("background-color: %1;\ncolor: %2;").arg(color.name()).arg((color.toHsl().lightnessF() > 0.5) ? "black" : "white"));
    }
}

void LXQtSysStatColors::on_buttons_clicked(QAbstractButton *button)
{
    switch (ui->buttons->standardButton(button))
    {
    case QDialogButtonBox::RestoreDefaults:
        restoreDefaults();
        break;

    case QDialogButtonBox::Reset:
        reset();
        break;

    case QDialogButtonBox::Ok:
        apply();
        accept();
        break;

    case QDialogButtonBox::Apply:
        apply();
        break;

    case QDialogButtonBox::Cancel:
        reset();
        reject();
        break;

    default:;
    }
}

void LXQtSysStatColors::restoreDefaults()
{
    bool wereTheSame = mColors == mDefaultColors;

    mColors = mDefaultColors;
    applyColorsToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(!wereTheSame);
}

void LXQtSysStatColors::reset()
{
    bool wereTheSame = mColors == mInitialColors;

    mColors = mInitialColors;
    applyColorsToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(!wereTheSame);
}

void LXQtSysStatColors::apply()
{
    emit colorsChanged();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
}

LXQtSysStatColors::Colors LXQtSysStatColors::colors() const
{
    return mColors;
}

LXQtSysStatColors::Colors LXQtSysStatColors::defaultColors() const
{
    return mDefaultColors;
}
