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

#include "configstyling.h"
#include "ui_configstyling.h"

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

ConfigStyling::ConfigStyling(LXQtPanel *panel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigStyling),
    mPanel(panel)
{
    ui->setupUi(this);
    fillComboBox_icon();

    mOldFontColor = mPanel->fontColor();
    mFontColor = mOldFontColor;
    mOldBackgroundColor = mPanel->backgroundColor();
    mBackgroundColor = mOldBackgroundColor;
    mOldBackgroundImage = mPanel->backgroundImage();
    mOldOpacity = mPanel->opacity();

    // reset configurations from file
    reset();

    connect(ui->checkBox_customFontColor,   &QCheckBox::toggled,       this, &ConfigStyling::editChanged);
    connect(ui->pushButton_customFontColor, &QPushButton::clicked,     this, &ConfigStyling::pickFontColor);
    connect(ui->checkBox_customBgColor,     &QCheckBox::toggled,       this, &ConfigStyling::editChanged);
    connect(ui->pushButton_customBgColor,   &QPushButton::clicked,     this, &ConfigStyling::pickBackgroundColor);
    connect(ui->checkBox_customBgImage,     &QCheckBox::toggled,       this, &ConfigStyling::editChanged);
    connect(ui->lineEdit_customBgImage,     &QLineEdit::textChanged,   this, &ConfigStyling::editChanged);
    connect(ui->pushButton_customBgImage,   &QPushButton::clicked,     this, &ConfigStyling::pickBackgroundImage);
    connect(ui->slider_opacity,             &QSlider::valueChanged,    this, &ConfigStyling::editChanged);
    connect(ui->groupBox_icon,              &QGroupBox::clicked,                       this, &ConfigStyling::editChanged);
    connect(ui->comboBox_icon,              QOverload<int>::of(&QComboBox::activated), this, &ConfigStyling::editChanged);
}


/************************************************
 *
 ************************************************/
void ConfigStyling::reset()
{
    mFontColor.setNamedColor(mOldFontColor.name());
    ui->pushButton_customFontColor->setStyleSheet(QStringLiteral("background: %1").arg(mOldFontColor.name()));
    mBackgroundColor.setNamedColor(mOldBackgroundColor.name());
    ui->pushButton_customBgColor->setStyleSheet(QStringLiteral("background: %1").arg(mOldBackgroundColor.name()));
    ui->lineEdit_customBgImage->setText(mOldBackgroundImage);
    ui->slider_opacity->setValue(mOldOpacity);

    ui->checkBox_customFontColor->setChecked(mOldFontColor.isValid());
    ui->checkBox_customBgColor->setChecked(mOldBackgroundColor.isValid());
    ui->checkBox_customBgImage->setChecked(QFileInfo::exists(mOldBackgroundImage));
}

/************************************************
 *
 ************************************************/
void ConfigStyling::fillComboBox_icon()
{
    ui->groupBox_icon->setChecked(!mPanel->iconTheme().isEmpty());

    QStringList themeList;
    QStringList processed;
    const QStringList baseDirs = QIcon::themeSearchPaths();
    for (const QString &baseDirName : baseDirs)
    {
        QDir baseDir(baseDirName);
        if (!baseDir.exists())
            continue;
        const QFileInfoList dirs = baseDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
        for (const QFileInfo &dir : dirs)
        {
            if (!processed.contains(dir.canonicalFilePath()))
            {
                processed << dir.canonicalFilePath();
                QDir Dir(dir.canonicalFilePath());
                QSettings file(Dir.absoluteFilePath(QStringLiteral("index.theme")), QSettings::IniFormat);
                if (file.status() == QSettings::NoError
                    && !file.value(QStringLiteral("Icon Theme/Directories")).toStringList().join(QLatin1Char(' ')).isEmpty()
                    && !file.value(QStringLiteral("Icon Theme/Hidden"), false).toBool())
                {
                    themeList << Dir.dirName();
                }
            }
        }
    }
    if (!themeList.isEmpty())
    {
        themeList.sort();
        ui->comboBox_icon->insertItems(0, themeList);
        QString curTheme = QIcon::themeName();
        if (!curTheme.isEmpty())
            ui->comboBox_icon->setCurrentText(curTheme);
    }
}


/************************************************
 *
 ************************************************/
void ConfigStyling::updateIconThemeSettings()
{
    ui->groupBox_icon->setChecked(!mPanel->iconTheme().isEmpty());
    QString curTheme = QIcon::themeName();
    if (!curTheme.isEmpty())
        ui->comboBox_icon->setCurrentText(curTheme);
}


/************************************************
 *
 ************************************************/
ConfigStyling::~ConfigStyling()
{
    delete ui;
}


/************************************************
 *
 ************************************************/
void ConfigStyling::editChanged()
{
    mPanel->setFontColor(ui->checkBox_customFontColor->isChecked() ? mFontColor : QColor(), true);
    if (ui->checkBox_customBgColor->isChecked())
    {
        mPanel->setBackgroundColor(mBackgroundColor, true);
        mPanel->setOpacity(ui->slider_opacity->value(), true);
    }
    else
    {
        mPanel->setBackgroundColor(QColor(), true);
        mPanel->setOpacity(100, true);
    }

    QString image = ui->checkBox_customBgImage->isChecked() ? ui->lineEdit_customBgImage->text() : QString();
    mPanel->setBackgroundImage(image, true);

    if (!ui->groupBox_icon->isChecked())
        mPanel->setIconTheme(QString());
    else if (!ui->comboBox_icon->currentText().isEmpty())
        mPanel->setIconTheme(ui->comboBox_icon->currentText());
}


/************************************************
 *
 ************************************************/
void ConfigStyling::pickFontColor()
{
    QColorDialog d(QColor(mFontColor.name()), this);
    d.setWindowTitle(tr("Pick color"));
    d.setWindowModality(Qt::WindowModal);
    if (d.exec() && d.currentColor().isValid())
    {
        mFontColor.setNamedColor(d.currentColor().name());
        ui->pushButton_customFontColor->setStyleSheet(QStringLiteral("background: %1").arg(mFontColor.name()));
        editChanged();
    }
}

/************************************************
 *
 ************************************************/
void ConfigStyling::pickBackgroundColor()
{
    QColorDialog d(QColor(mBackgroundColor.name()), this);
    d.setWindowTitle(tr("Pick color"));
    d.setWindowModality(Qt::WindowModal);
    if (d.exec() && d.currentColor().isValid())
    {
        mBackgroundColor.setNamedColor(d.currentColor().name());
        ui->pushButton_customBgColor->setStyleSheet(QStringLiteral("background: %1").arg(mBackgroundColor.name()));
        editChanged();
    }
}

/************************************************
 *
 ************************************************/
void ConfigStyling::pickBackgroundImage()
{
    QString picturesLocation;
    picturesLocation = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    QFileDialog* d = new QFileDialog(this, tr("Pick image"), picturesLocation, tr("Images (*.png *.gif *.jpg)"));
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->setWindowModality(Qt::WindowModal);
    connect(d, &QFileDialog::fileSelected, ui->lineEdit_customBgImage, &QLineEdit::setText);
    d->show();
}
