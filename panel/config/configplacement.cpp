/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "configplacement.h"
#include "ui_configplacement.h"

#include "../lxqtpanellimits.h"

#include <KWindowSystem/KWindowSystem>
#include <QDebug>
#include <QListView>
#include <QScreen>
#include <QWindow>
#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>

using namespace LXQt;

struct ScreenPosition
{
    int screen;
    ILXQtPanel::Position position;
};
Q_DECLARE_METATYPE(ScreenPosition)

ConfigPlacement::ConfigPlacement(LXQtPanel *panel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPlacement),
    mPanel(panel)
{
    ui->setupUi(this);

    fillComboBox_position();
    fillComboBox_alignment();

    mOldPanelSize = mPanel->panelSize();
    mOldIconSize = mPanel->iconSize();
    mOldLineCount = mPanel->lineCount();

    mOldLength = mPanel->length();
    mOldLengthInPercents = mPanel->lengthInPercents();

    mOldAlignment = mPanel->alignment();

    mOldScreenNum = mPanel->screenNum();
    mScreenNum = mOldScreenNum;

    mOldPosition = mPanel->position();
    mPosition = mOldPosition;

    mOldHidable = mPanel->hidable();

    mOldVisibleMargin = mPanel->visibleMargin();

    mOldHideOnOverlap = mPanel->hideOnOverlap();

    mOldAnimation = mPanel->animationTime();
    mOldShowDelay = mPanel->showDelay();

    ui->spinBox_panelSize->setMinimum(PANEL_MINIMUM_SIZE);
    ui->spinBox_panelSize->setMaximum(PANEL_MAXIMUM_SIZE);

    mOldReserveSpace = mPanel->reserveSpace();

    // reset configurations from file
    reset();

    connect(ui->spinBox_panelSize,          QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);
    connect(ui->spinBox_iconSize,           QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);
    connect(ui->spinBox_lineCount,          QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);

    connect(ui->spinBox_length,             QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);
    connect(ui->comboBox_lengthType,        QOverload<int>::of(&QComboBox::activated),        this, &ConfigPlacement::widthTypeChanged);

    connect(ui->comboBox_alignment,         QOverload<int>::of(&QComboBox::activated),        this, &ConfigPlacement::editChanged);
    connect(ui->comboBox_position,          QOverload<int>::of(&QComboBox::activated),        this, &ConfigPlacement::positionChanged);
    connect(ui->groupBox_hidable,           &QGroupBox::toggled,                              this, &ConfigPlacement::editChanged);
    connect(ui->checkBox_visibleMargin,     &QCheckBox::toggled,                              this, &ConfigPlacement::editChanged);
    connect(ui->checkBox_overlap,           &QAbstractButton::toggled,                        this, &ConfigPlacement::editChanged);
    connect(ui->spinBox_animation,          QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);
    connect(ui->spinBox_delay,              QOverload<int>::of(&QSpinBox::valueChanged),      this, &ConfigPlacement::editChanged);

    connect(ui->checkBox_reserveSpace,      &QAbstractButton::toggled, this, [this](bool checked) { mPanel->setReserveSpace(checked, true); });

}


/************************************************
 *
 ************************************************/
void ConfigPlacement::reset()
{
    ui->spinBox_panelSize->setValue(mOldPanelSize);
    ui->spinBox_iconSize->setValue(mOldIconSize);
    ui->spinBox_lineCount->setValue(mOldLineCount);

    ui->comboBox_position->setCurrentIndex(indexForPosition(mOldScreenNum, mOldPosition));

    ui->groupBox_hidable->setChecked(mOldHidable);

    ui->checkBox_visibleMargin->setChecked(mOldVisibleMargin);

    ui->checkBox_overlap->setChecked(mOldHideOnOverlap);

    ui->spinBox_animation->setValue(mOldAnimation);
    ui->spinBox_delay->setValue(mOldShowDelay);

    fillComboBox_alignment();
    ui->comboBox_alignment->setCurrentIndex(mOldAlignment + 1);

    ui->comboBox_lengthType->setCurrentIndex(mOldLengthInPercents ? 0 : 1);
    widthTypeChanged();
    ui->spinBox_length->setValue(mOldLength);

    ui->checkBox_reserveSpace->setChecked(mOldReserveSpace);

    // update position
    positionChanged();
}

/************************************************
 *
 ************************************************/
void ConfigPlacement::fillComboBox_position()
{
    int screenCount = QApplication::screens().size();
    if (screenCount == 1)
    {
        addPosition(tr("Top of desktop"), 0, LXQtPanel::PositionTop);
        addPosition(tr("Left of desktop"), 0, LXQtPanel::PositionLeft);
        addPosition(tr("Right of desktop"), 0, LXQtPanel::PositionRight);
        addPosition(tr("Bottom of desktop"), 0, LXQtPanel::PositionBottom);
    }
    else
    {
        for (int screenNum = 0; screenNum < screenCount; screenNum++)
        {
            if (screenNum)
                ui->comboBox_position->insertSeparator(9999);

            addPosition(tr("Top of desktop %1").arg(screenNum +1), screenNum, LXQtPanel::PositionTop);
            addPosition(tr("Left of desktop %1").arg(screenNum +1), screenNum, LXQtPanel::PositionLeft);
            addPosition(tr("Right of desktop %1").arg(screenNum +1), screenNum, LXQtPanel::PositionRight);
            addPosition(tr("Bottom of desktop %1").arg(screenNum +1), screenNum, LXQtPanel::PositionBottom);
        }
    }
}


/************************************************
 *
 ************************************************/
void ConfigPlacement::fillComboBox_alignment()
{
    ui->comboBox_alignment->setItemData(0, QVariant(LXQtPanel::AlignmentLeft));
    ui->comboBox_alignment->setItemData(1, QVariant(LXQtPanel::AlignmentCenter));
    ui->comboBox_alignment->setItemData(2,  QVariant(LXQtPanel::AlignmentRight));


    if (mPosition   == ILXQtPanel::PositionTop ||
        mPosition   == ILXQtPanel::PositionBottom)
    {
        ui->comboBox_alignment->setItemText(0, tr("Left"));
        ui->comboBox_alignment->setItemText(1, tr("Center"));
        ui->comboBox_alignment->setItemText(2, tr("Right"));
    }
    else
    {
        ui->comboBox_alignment->setItemText(0, tr("Top"));
        ui->comboBox_alignment->setItemText(1, tr("Center"));
        ui->comboBox_alignment->setItemText(2, tr("Bottom"));
    };
}


/************************************************
 *
 ************************************************/
void ConfigPlacement::addPosition(const QString& name, int screen, LXQtPanel::Position position)
{
    if (LXQtPanel::canPlacedOn(screen, position))
        ui->comboBox_position->addItem(name, QVariant::fromValue(ScreenPosition{screen, position}));
}


/************************************************
 *
 ************************************************/
int ConfigPlacement::indexForPosition(int screen, ILXQtPanel::Position position)
{
    for (int i = 0; i < ui->comboBox_position->count(); i++)
    {
        ScreenPosition sp = ui->comboBox_position->itemData(i).value<ScreenPosition>();
        if (screen == sp.screen && position == sp.position)
            return i;
    }
    return -1;
}


/************************************************
 *
 ************************************************/
ConfigPlacement::~ConfigPlacement()
{
    delete ui;
}


/************************************************
 *
 ************************************************/
void ConfigPlacement::editChanged()
{
    mPanel->setPanelSize(ui->spinBox_panelSize->value(), true);
    mPanel->setIconSize(ui->spinBox_iconSize->value(), true);
    mPanel->setLineCount(ui->spinBox_lineCount->value(), true);

    mPanel->setLength(ui->spinBox_length->value(),
                      ui->comboBox_lengthType->currentIndex() == 0,
                      true);

    LXQtPanel::Alignment align = LXQtPanel::Alignment(
        ui->comboBox_alignment->itemData(
            ui->comboBox_alignment->currentIndex()
        ).toInt());

    mPanel->setAlignment(align, true);
    mPanel->setPosition(mScreenNum, mPosition, true);
    mPanel->setHidable(ui->groupBox_hidable->isChecked(), true);
    mPanel->setVisibleMargin(ui->checkBox_visibleMargin->isChecked(), true);
    mPanel->setHideOnOverlap(ui->checkBox_overlap->isChecked(), true);
    mPanel->setAnimationTime(ui->spinBox_animation->value(), true);
    mPanel->setShowDelay(ui->spinBox_delay->value(), true);
}


/************************************************
 *
 ************************************************/
void ConfigPlacement::widthTypeChanged()
{
    int max = getMaxLength();

    if (ui->comboBox_lengthType->currentIndex() == 0)
    {
        // Percents .............................
        int v = ui->spinBox_length->value() * 100.0 / max;
        ui->spinBox_length->setRange(1, 100);
        ui->spinBox_length->setValue(v);
    }
    else
    {
        // Pixels ...............................
        int v =  max / 100.0 * ui->spinBox_length->value();
        ui->spinBox_length->setRange(-max, max);
        ui->spinBox_length->setValue(v);
    }
}


/************************************************
 *
 ************************************************/
int ConfigPlacement::getMaxLength()
{
    auto screens = QApplication::screens();
    if (screens.size() > mScreenNum)
    {
        if (mPosition == ILXQtPanel::PositionTop ||
            mPosition == ILXQtPanel::PositionBottom)
            return screens.at(mScreenNum)->geometry().width();
        else
            return screens.at(mScreenNum)->geometry().height();
    }
    return 0;
}


/************************************************
 *
 ************************************************/
void ConfigPlacement::positionChanged()
{
    ScreenPosition sp = ui->comboBox_position->itemData(
        ui->comboBox_position->currentIndex()).value<ScreenPosition>();

        bool updateAlig = (sp.position == ILXQtPanel::PositionTop ||
        sp.position == ILXQtPanel::PositionBottom) !=
        (mPosition   == ILXQtPanel::PositionTop ||
        mPosition   == ILXQtPanel::PositionBottom);

        int oldMax = getMaxLength();
        mPosition = sp.position;
        mScreenNum = sp.screen;
        int newMax = getMaxLength();

        if (ui->comboBox_lengthType->currentIndex() == 1 &&
            oldMax != newMax)
        {
            // Pixels ...............................
            int v = ui->spinBox_length->value() * 1.0 * newMax / oldMax;
            ui->spinBox_length->setMaximum(newMax);
            ui->spinBox_length->setValue(v);
        }

        if (updateAlig)
            fillComboBox_alignment();

        editChanged();
}
