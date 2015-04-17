/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

#include "configpanelwidget.h"
#include "ui_configpanelwidget.h"

#include "../lxqtpanellimits.h"

#include <KWindowSystem/KWindowSystem>
#include <QDebug>
#include <QListView>
#include <QDesktopWidget>
#include <QWindow>
#include <QColorDialog>
#include <QFileDialog>
#include <QStandardPaths>

using namespace LxQt;

struct ScreenPosition
{
    int screen;
    ILxQtPanel::Position position;
};
Q_DECLARE_METATYPE(ScreenPosition)

ConfigPanelWidget::ConfigPanelWidget(LxQtPanel *panel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanelWidget),
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

    ui->spinBox_panelSize->setMinimum(PANEL_MINIMUM_SIZE);
    ui->spinBox_panelSize->setMaximum(PANEL_MAXIMUM_SIZE);

    mOldFontColor = mPanel->fontColor();
    mFontColor = mOldFontColor;
    mOldBackgroundColor = mPanel->backgroundColor();
    mBackgroundColor = mOldBackgroundColor;
    mOldBackgroundImage = mPanel->backgroundImage();
    mOldOpacity = mPanel->opacity();

    // reset configurations from file
    reset();

    connect(ui->spinBox_panelSize,          SIGNAL(valueChanged(int)),      this, SLOT(editChanged()));
    connect(ui->spinBox_iconSize,           SIGNAL(valueChanged(int)),      this, SLOT(editChanged()));
    connect(ui->spinBox_lineCount,          SIGNAL(valueChanged(int)),      this, SLOT(editChanged()));

    connect(ui->spinBox_length,             SIGNAL(valueChanged(int)),      this, SLOT(editChanged()));
    connect(ui->comboBox_lenghtType,        SIGNAL(activated(int)),         this, SLOT(widthTypeChanged()));

    connect(ui->comboBox_alignment,         SIGNAL(activated(int)),         this, SLOT(editChanged()));
    connect(ui->comboBox_position,          SIGNAL(activated(int)),         this, SLOT(positionChanged()));
    connect(ui->checkBox_hidable,           SIGNAL(toggled(bool)),          this, SLOT(editChanged()));

    connect(ui->checkBox_customFontColor,   SIGNAL(toggled(bool)),          this, SLOT(editChanged()));
    connect(ui->pushButton_customFontColor, SIGNAL(clicked(bool)),          this, SLOT(pickFontColor()));
    connect(ui->checkBox_customBgColor,     SIGNAL(toggled(bool)),          this, SLOT(editChanged()));
    connect(ui->pushButton_customBgColor,   SIGNAL(clicked(bool)),          this, SLOT(pickBackgroundColor()));
    connect(ui->checkBox_customBgImage,     SIGNAL(toggled(bool)),          this, SLOT(editChanged()));
    connect(ui->lineEdit_customBgImage,     SIGNAL(textChanged(QString)),   this, SLOT(editChanged()));
    connect(ui->pushButton_customBgImage,   SIGNAL(clicked(bool)),          this, SLOT(pickBackgroundImage()));
    connect(ui->slider_opacity,             SIGNAL(sliderReleased()),       this, SLOT(editChanged()));
}


/************************************************
 *
 ************************************************/
void ConfigPanelWidget::reset()
{
    ui->spinBox_panelSize->setValue(mOldPanelSize);
    ui->spinBox_iconSize->setValue(mOldIconSize);
    ui->spinBox_lineCount->setValue(mOldLineCount);

    ui->comboBox_position->setCurrentIndex(indexForPosition(mOldScreenNum, mOldPosition));

    ui->checkBox_hidable->setChecked(mOldHidable);

    fillComboBox_alignment();
    ui->comboBox_alignment->setCurrentIndex(mOldAlignment + 1);

    ui->comboBox_lenghtType->setCurrentIndex(mOldLengthInPercents ? 0 : 1);
    widthTypeChanged();
    ui->spinBox_length->setValue(mOldLength);

    mFontColor.setNamedColor(mOldFontColor.name());
    ui->pushButton_customFontColor->setStyleSheet(QString("background: %1").arg(mOldFontColor.name()));
    mBackgroundColor.setNamedColor(mOldBackgroundColor.name());
    ui->pushButton_customBgColor->setStyleSheet(QString("background: %1").arg(mOldBackgroundColor.name()));
    ui->lineEdit_customBgImage->setText(mOldBackgroundImage);
    ui->slider_opacity->setValue(mOldOpacity);

    ui->checkBox_customFontColor->setChecked(mOldFontColor.isValid());
    ui->checkBox_customBgColor->setChecked(mOldBackgroundColor.isValid());
    ui->checkBox_customBgImage->setChecked(QFileInfo(mOldBackgroundImage).exists());

    // update position
    positionChanged();
}

/************************************************
 *
 ************************************************/
void ConfigPanelWidget::fillComboBox_position()
{
    int screenCount = QApplication::desktop()->screenCount();
    if (screenCount == 1)
    {
        addPosition(tr("Top of desktop"), 0, LxQtPanel::PositionTop);
        addPosition(tr("Left of desktop"), 0, LxQtPanel::PositionLeft);
        addPosition(tr("Right of desktop"), 0, LxQtPanel::PositionRight);
        addPosition(tr("Bottom of desktop"), 0, LxQtPanel::PositionBottom);
    }
    else
    {
        for (int screenNum = 0; screenNum < screenCount; screenNum++)
        {
            if (screenNum)
                ui->comboBox_position->insertSeparator(9999);

            addPosition(tr("Top of desktop %1").arg(screenNum +1), screenNum, LxQtPanel::PositionTop);
            addPosition(tr("Left of desktop %1").arg(screenNum +1), screenNum, LxQtPanel::PositionLeft);
            addPosition(tr("Right of desktop %1").arg(screenNum +1), screenNum, LxQtPanel::PositionRight);
            addPosition(tr("Bottom of desktop %1").arg(screenNum +1), screenNum, LxQtPanel::PositionBottom);
        }
    }
}


/************************************************
 *
 ************************************************/
void ConfigPanelWidget::fillComboBox_alignment()
{
    ui->comboBox_alignment->setItemData(0, QVariant(LxQtPanel::AlignmentLeft));
    ui->comboBox_alignment->setItemData(1, QVariant(LxQtPanel::AlignmentCenter));
    ui->comboBox_alignment->setItemData(2,  QVariant(LxQtPanel::AlignmentRight));


    if (mPosition   == ILxQtPanel::PositionTop ||
        mPosition   == ILxQtPanel::PositionBottom)
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
void ConfigPanelWidget::addPosition(const QString& name, int screen, LxQtPanel::Position position)
{
    if (LxQtPanel::canPlacedOn(screen, position))
        ui->comboBox_position->addItem(name, QVariant::fromValue((ScreenPosition){screen, position}));
}


/************************************************
 *
 ************************************************/
int ConfigPanelWidget::indexForPosition(int screen, ILxQtPanel::Position position)
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
ConfigPanelWidget::~ConfigPanelWidget()
{
    delete ui;
}


/************************************************
 *
 ************************************************/
void ConfigPanelWidget::editChanged()
{
    mPanel->setPanelSize(ui->spinBox_panelSize->value(), true);
    mPanel->setIconSize(ui->spinBox_iconSize->value(), true);
    mPanel->setLineCount(ui->spinBox_lineCount->value(), true);

    mPanel->setLength(ui->spinBox_length->value(),
                      ui->comboBox_lenghtType->currentIndex() == 0,
                      true);

    LxQtPanel::Alignment align = LxQtPanel::Alignment(
        ui->comboBox_alignment->itemData(
            ui->comboBox_alignment->currentIndex()
        ).toInt());

    mPanel->setAlignment(align, true);
    mPanel->setPosition(mScreenNum, mPosition, true);
    mPanel->setHidable(ui->checkBox_hidable->isChecked(), true);

    mPanel->setFontColor(ui->checkBox_customFontColor->isChecked() ? mFontColor : QColor(), true);
    if (ui->checkBox_customBgColor->isChecked())
    {
        mPanel->setBackgroundColor(mBackgroundColor, true);
        mPanel->setOpacity(ui->slider_opacity->value(), true);
    }
    else
    {
        mPanel->setBackgroundColor(QColor(), true);
        mPanel->setOpacity(-1, true);
    }

    QString image = ui->checkBox_customBgImage->isChecked() ? ui->lineEdit_customBgImage->text() : QString();
    mPanel->setBackgroundImage(image, true);
}


/************************************************
 *
 ************************************************/
void ConfigPanelWidget::widthTypeChanged()
{
    int max = getMaxLength();

    if (ui->comboBox_lenghtType->currentIndex() == 0)
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
int ConfigPanelWidget::getMaxLength()
{
    QDesktopWidget* dw = QApplication::desktop();

    if (mPosition == ILxQtPanel::PositionTop ||
        mPosition == ILxQtPanel::PositionBottom)
        return dw->screenGeometry(mScreenNum).width();
    else
        return dw->screenGeometry(mScreenNum).height();
}


/************************************************
 *
 ************************************************/
void ConfigPanelWidget::positionChanged()
{
    ScreenPosition sp = ui->comboBox_position->itemData(
        ui->comboBox_position->currentIndex()).value<ScreenPosition>();

        bool updateAlig = (sp.position == ILxQtPanel::PositionTop ||
        sp.position == ILxQtPanel::PositionBottom) !=
        (mPosition   == ILxQtPanel::PositionTop ||
        mPosition   == ILxQtPanel::PositionBottom);

        int oldMax = getMaxLength();
        mPosition = sp.position;
        mScreenNum = sp.screen;
        int newMax = getMaxLength();

        if (ui->comboBox_lenghtType->currentIndex() == 1 &&
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

/************************************************
 *
 ************************************************/
void ConfigPanelWidget::pickFontColor()
{
    QColorDialog d(QColor(mFontColor.name()), this);
    d.setWindowTitle(tr("Pick color"));
    d.setWindowModality(Qt::WindowModal);
    if (d.exec() && d.currentColor().isValid())
    {
        mFontColor.setNamedColor(d.currentColor().name());
        ui->pushButton_customFontColor->setStyleSheet(QString("background: %1").arg(mFontColor.name()));
        editChanged();
    }
}

/************************************************
 *
 ************************************************/
void ConfigPanelWidget::pickBackgroundColor()
{
    QColorDialog d(QColor(mBackgroundColor.name()), this);
    d.setWindowTitle(tr("Pick color"));
    d.setWindowModality(Qt::WindowModal);
    if (d.exec() && d.currentColor().isValid())
    {
        mBackgroundColor.setNamedColor(d.currentColor().name());
        ui->pushButton_customBgColor->setStyleSheet(QString("background: %1").arg(mBackgroundColor.name()));
        editChanged();
    }
}

/************************************************
 *
 ************************************************/
void ConfigPanelWidget::pickBackgroundImage()
{
    QString picturesLocation;
    picturesLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    QFileDialog* d = new QFileDialog(this, tr("Pick image"), picturesLocation, tr("Images (*.png *.gif *.jpg)"));
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->setWindowModality(Qt::WindowModal);
    connect(d, &QFileDialog::fileSelected, ui->lineEdit_customBgImage, &QLineEdit::setText);
    d->show();
}

