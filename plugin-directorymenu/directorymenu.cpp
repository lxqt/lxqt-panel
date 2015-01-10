/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Daniel Drzisga <sersmicro@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is diinstributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <vector>

#include "directorymenu.h"
#include <QDebug>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QProcess>

#include <XdgIcon>

DirectoryMenu::DirectoryMenu(const ILxQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mMenu(0)
{
    mOpenDirectorySignalMapper = new QSignalMapper(this);
    mOpenTerminalSignalMapper = new QSignalMapper(this);
    mMenuSignalMapper = new QSignalMapper(this);

    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mButton.setIcon(XdgIcon::fromTheme("folder"));
    mButton.installEventFilter(this);

    connect(&mButton, SIGNAL(clicked()), this, SLOT(showMenu()));
    connect(mOpenDirectorySignalMapper, SIGNAL(mapped(QString)), this, SLOT(openDirectory(QString)));
    connect(mOpenTerminalSignalMapper, SIGNAL(mapped(QString)), this, SLOT(openTerminal(QString)));
    connect(mMenuSignalMapper, SIGNAL(mapped(QString)), this, SLOT(addMenu(QString)));
}

DirectoryMenu::~DirectoryMenu()
{
	if(mMenu)
	{
		delete mMenu;
		mMenu = 0;
	}
}

void DirectoryMenu::showMenu()
{
    buildMenu(QDir::homePath());

    int x=0, y=0;

    switch (panel()->position())
    {
        case ILxQtPanel::PositionTop:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().bottom();
            break;

        case ILxQtPanel::PositionBottom:
            x = mButton.mapToGlobal(QPoint(0, 0)).x();
            y = panel()->globalGometry().top() - mMenu->sizeHint().height();
            break;

        case ILxQtPanel::PositionLeft:
            x = panel()->globalGometry().right();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;

        case ILxQtPanel::PositionRight:
            x = panel()->globalGometry().left() - mMenu->sizeHint().width();
            y = mButton.mapToGlobal(QPoint(0, 0)).y();
            break;
    }

    // Just using Qt`s activateWindow() won't work on some WMs like Kwin.
    // Solution is to execute menu 1ms later using timer
    mButton.activateWindow();
    mMenu->exec(QPoint(x, y));
}

void DirectoryMenu::buildMenu(const QString& path)
{
	if(mMenu)
	{
		delete mMenu;
		mMenu = 0;
	}

	mPathStrings.clear();

	mMenu = new QMenu();

	addActions(mMenu, path);
}

void DirectoryMenu::openDirectory(const QString& path)
{
	QDesktopServices::openUrl(QUrl("file://" + QDir::toNativeSeparators(path)));
}

void DirectoryMenu::openTerminal(const QString& path)
{
	QString command = QString("exo-open --working-directory \"%1\" --launch TerminalEmulator").arg(path);
	QProcess::startDetached(command);
}

void DirectoryMenu::addMenu(QString path)
{
	QSignalMapper* sender = (QSignalMapper* )QObject::sender();
	QMenu* parentMenu = (QMenu*) sender->mapping(path);

	if(parentMenu->isEmpty())
	{
		addActions(parentMenu, path);
	}
}

void DirectoryMenu::addActions(QMenu* menu, const QString& path)
{
	mPathStrings.push_back(path);

	QAction* openDirectoryAction = menu->addAction(XdgIcon::fromTheme("folder"), tr("Open"));
	connect(openDirectoryAction, SIGNAL(triggered()), mOpenDirectorySignalMapper, SLOT(map()));
	mOpenDirectorySignalMapper->setMapping(openDirectoryAction, mPathStrings.back());

	QAction* openTerminalAction = menu->addAction(XdgIcon::fromTheme("utilities-terminal"), tr("Open in terminal"));
	connect(openTerminalAction, SIGNAL(triggered()), mOpenTerminalSignalMapper, SLOT(map()));
	mOpenTerminalSignalMapper->setMapping(openTerminalAction, mPathStrings.back());

	menu->addSeparator();

	QDir dir(path);
	QFileInfoList list = dir.entryInfoList();

	foreach (const QFileInfo& entry, list)
    {
    	if(entry.isDir() && !entry.isHidden())
    	{
			mPathStrings.push_back(entry.fileName());

			QMenu* subMenu = menu->addMenu(XdgIcon::fromTheme("folder"), mPathStrings.back());

    		connect(subMenu, SIGNAL(aboutToShow()), mMenuSignalMapper, SLOT(map()));
    		mMenuSignalMapper->setMapping(subMenu, entry.absoluteFilePath());	
    	}
    }
}