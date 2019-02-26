/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Matteo Fois <giomatfois62@yahoo.it>
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
 
#include "stickynote.h"
#include "ui_stickynote.h"

#include <QFont>
#include <QMouseEvent>
#include <QDateTime>
#include <QSettings>
#include <QFontDialog>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLabel>


StickyNote::StickyNote(qint64 id, QWidget *parent):
    QWidget(parent),
    prevPosSet(false),
    mHasOwnFont(false),
    ui(new Ui::StickyNote)
{   
    ui->setupUi(this);

    QString buttonStyle = "QToolButton {"
            "border: 0px;"
            "border-radius: 0px;"
            "}";

    QString textStyle = "QTextEdit {"
            "border: 0px;"
            "border-radius: 0px;"
            "}";

    ui->deleteButton->setStyleSheet(buttonStyle);
    ui->fontButton->setStyleSheet(buttonStyle);
    ui->textEdit->setStyleSheet(textStyle);

    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(saveText())); 
    connect(ui->fontButton, SIGNAL(clicked()), this, SLOT(changeFont()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(requestDelete()));
    
    if(id) {
        mId = id;
        load();
    } else {
        mId = QDateTime::currentSecsSinceEpoch();
    }
    
    QDateTime timestamp;
    timestamp.setTime_t(mId);
    ui->title->setText(timestamp.toString("<b>dd.MM.yyyy-HH:mm</b>"));
}

StickyNote::~StickyNote()
{
    delete ui;
}

void StickyNote::changeFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont());
    
    if(ok) {
        // store choosen font
        QString dataFile = dataDir() + QDir::separator() + QString::number(mId);
        QSettings settings(dataFile, QSettings::IniFormat);
        settings.setValue("font", font);
        
        setFont(font);
        
        mHasOwnFont = true;
    }
}

void StickyNote::setFont(const QFont &font)
{
    ui->textEdit->setFont(font);
    //ui->title->setFont(font);
}

void StickyNote::setColors(const QString &backGround, const QString &foreground)
{
    // set bg color & textcolor
    QString widgetStyle = QString("StickyNote {"
                "background: %1;"
                "}").arg(backGround);
    
    QString textEditStyle = QString("QTextEdit {"
                "border: 0px;"
                "background: %1;"
                "}").arg(backGround);

    QString labelStyle = QString("QLabel {"
                "color: %1;"
                "}").arg(foreground);

    setStyleSheet(widgetStyle);
    ui->textEdit->setStyleSheet(textEditStyle);
    ui->textEdit->setTextColor(QColor(foreground));
    ui->title->setStyleSheet(labelStyle);
    
    // reset text
    QString dataFile = dataDir() + QDir::separator() + QString::number(mId);
    QSettings settings(dataFile, QSettings::IniFormat);
    ui->textEdit->setPlainText(settings.value("text","").toString());

    // set new icons
    QString iconsDir = dataDir() + QDir::separator() + "icons";
    QString deleteIcon = iconsDir + QDir::separator() + "times-solid.svg";
    QString fontIcons = iconsDir + QDir::separator() + "font-solid.svg";
    
    ui->deleteButton->setIcon(QIcon(deleteIcon));
    ui->fontButton->setIcon(QIcon(fontIcons));
}

void StickyNote::requestDelete()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Note"),
        tr("Delete this note?"),
        QMessageBox::Yes|QMessageBox::No);
    
    if(reply == QMessageBox::Yes)
        emit deleteRequested(mId);
}

void StickyNote::load()
{
    QString dataFile = dataDir() + QDir::separator() + QString::number(mId);
    QSettings settings(dataFile, QSettings::IniFormat);
    
    // load current position
    QSize size = qvariant_cast<QSize>(settings.value("size"));
    QPoint position = qvariant_cast<QPoint>(settings.value("position"));
    move(position);
    resize(size);
    
    // load current text    
    ui->textEdit->setPlainText(settings.value("text","").toString());

    // load font settings
    QString font = settings.value("font","").toString();    
    if(!font.isEmpty()) {
        QFont font = qvariant_cast<QFont>(settings.value("font"));
        ui->textEdit->setFont(font);
        
        mHasOwnFont = true;
    }
}

void StickyNote::saveText()
{
    QString dataFile = dataDir() + QDir::separator() + QString::number(mId);
    QSettings settings(dataFile, QSettings::IniFormat);
    settings.setValue("text", ui->textEdit->toPlainText());
}

void StickyNote::savePosition()
{
    QString dataFile = dataDir() + QDir::separator() + QString::number(mId);
    QSettings settings(dataFile, QSettings::IniFormat);
    settings.setValue("position",mapToGlobal(rect().topLeft()));
    settings.setValue("size",rect().size());
}

QString StickyNote::dataDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QDir::separator() + "lxqt-notes";
    return dir;
}

void StickyNote::resizeEvent(QResizeEvent *event)
{
    savePosition();
}

void StickyNote::moveEvent(QMoveEvent *event)
{
    savePosition();
}

void StickyNote::mouseMoveEvent(QMouseEvent *event)
{
    if(!prevPosSet) {
        prevPos = event->pos();
        prevPosSet = true;
    } else {
        QRect geometry = this->geometry();
        QPoint pos = event->pos() - prevPos;
        move(x() + pos.x(), y() + pos.y());
    }
}

void StickyNote::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(prevPosSet) {
            prevPosSet = false;
            savePosition();
        }
    }
}
