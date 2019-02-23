/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Aaron Lewis <the.warl0ck.1989@gmail.com>
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

#include "notes.h"
#include "notesconfiguration.h"
#include "stickynote.h"

#include <QMouseEvent>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScreen>
#include <QStandardPaths>
#include <QSettings>
#include <QLabel>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>
#include <QFontDialog>
#include <QAction>
#include <QMenu>
#include <QDebug>

Notes::Notes(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    mHidden(false)
{
    realign();
    
    mButton.setAutoRaise(true);
    mButton.setIcon(XdgIcon::fromTheme("date", "date"));
    
    QAction *addNew = new QAction(tr("New Note"));
    QAction *showHide = new QAction(tr("Show/Hide Notes"));
    connect(addNew, SIGNAL(triggered()), this, SLOT(addNewNote()));
    connect(showHide, SIGNAL(triggered()), this, SLOT(toggleShowHide()));
    
    QMenu *menu = new QMenu;
    menu->addAction(addNew);
    menu->addAction(showHide);
    mButton.setMenu(menu);
    mButton.setPopupMode(QToolButton::InstantPopup);
    
    QString showOnStartup = settings()->value("showNotesOnStartup","").toString();
	if(!showOnStartup.isEmpty())
    	mHidden = !settings()->value("showNotesOnStartup").toBool();
    
    // load all notes
    QDir dir(dataDir());
	QStringList notes = dir.entryList(QDir::Files);
    for(int i = 0; i < notes.size(); ++i) {
    	qint64 noteId = notes[i].toLong();
    	if(!noteId)
    		continue;
    	
    	StickyNote *note = new StickyNote(noteId);
    	connect(note, &StickyNote::deleteRequested, this, &Notes::deleteNote);
    	
    	mNotes[noteId] = note;
    }
    
    // set colors & fonts 
    settingsChanged();
}


Notes::~Notes()
{
	for(auto it = mNotes.begin(); it != mNotes.end(); ++it)
    	delete it.value();
    
	mNotes.clear();
}

void Notes::realign()
{
	mButton.setFixedHeight(panel()->iconSize());
    mButton.setFixedWidth(panel()->iconSize());
}


QString Notes::dataDir()
{
	QString dir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QDir::separator() + "lxqt-notes";
	return dir;
}

void Notes::addNewNote()
{
	StickyNote *note = new StickyNote();
	connect(note, &StickyNote::deleteRequested, this, &Notes::deleteNote);
	
	QFont font = qvariant_cast<QFont>(settings()->value("defaultFont"));
	QString bgColor = settings()->value("backgroundColor", "#ffff7f").toString();
	QString fgColor = settings()->value("foregroundColor", "#000000").toString();
	bool showFrame = settings()->value("showWindowFrame","false").toBool();
	
	note->setFont(font);
    note->setColors(bgColor, fgColor);
    
    if(!showFrame)
    	note->setWindowFlags(Qt::Window | Qt::Tool | Qt::FramelessWindowHint);
    else
    	note->setWindowFlags(Qt::Window | Qt::Tool);
	
	mNotes[note->id()] = note;
	note->show();
}

void Notes::deleteNote(const qint64 &id)
{
	auto it = mNotes.find(id);
	if(it != mNotes.end()) {
		delete it.value();
		mNotes.erase(it);
	}
	
	// delete note file
	QDir dir(dataDir());
    dir.remove(QString::number(id));
}

void Notes::toggleShowHide()
{
	// TODO: call note function to move to or store last known position
	if(mHidden) {
		for(auto it = mNotes.begin(); it != mNotes.end(); ++it)
    		it.value()->show();
    	mHidden = false;
	} else {
		for(auto it = mNotes.begin(); it != mNotes.end(); ++it)
    		it.value()->hide();
    	mHidden = true;
	}
}

void Notes::setIconsColor(const QString &color)
{
	// create icons path
	QString iconsDir = dataDir() + QDir::separator() + "icons";
	QDir().mkpath(iconsDir);

	// set delete icon color
    QFile file(":/resources/times-solid-template.svg");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Could not open file for read";
        return;
    }
    
    QTextStream in(&file);
    QString svg = in.readAll().arg(color);
    
    file.close();

	QString iconFile = iconsDir + QDir::separator() + "times-solid.svg";
	
    QFile outFile(iconFile);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not open file for write";
        return;
    }
    
    QTextStream out(&outFile);
    out << svg;
    
    outFile.close();

	// set font icon color
	QFile file2(":/resources/font-solid-template.svg");

    if(!file2.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Could not open file for read";
        return;
    }

    QTextStream in2(&file2);
    svg = in2.readAll().arg(color);
    
    file2.close();

	iconFile = iconsDir + QDir::separator() + "font-solid.svg";
    
    QFile outFile2(iconFile);
    if(!outFile2.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "Could not open file for read";
        return;
    }
    
    QTextStream out2(&outFile2);
    out2 << svg;
    
    outFile2.close();
}

void Notes::settingsChanged()
{
	// set new params
	QFont font = qvariant_cast<QFont>(settings()->value("defaultFont"));
	QString bgColor = settings()->value("backgroundColor", "#ffff7f").toString();
	QString fgColor = settings()->value("foregroundColor", "#000000").toString();
	bool showFrame = settings()->value("showWindowFrame","false").toBool();
	
	setIconsColor(fgColor);
	
	for(auto it = mNotes.begin(); it != mNotes.end(); ++it) {
    	StickyNote *note = it.value();
    	if(!note->hasOwnFont())
    		note->setFont(font);
    		
    	note->setColors(bgColor, fgColor);
    	
    	if(!showFrame)
    		note->setWindowFlags(Qt::Window | Qt::Tool | Qt::FramelessWindowHint);
    	else
    		note->setWindowFlags(Qt::Window | Qt::Tool);
    	
    	if(!mHidden)
    		note->show();
    }
}

QDialog* Notes::configureDialog()
{
	NotesConfiguration *configDialog = new NotesConfiguration(settings());
	configDialog->setAttribute(Qt::WA_DeleteOnClose, true);
	return configDialog;
}
