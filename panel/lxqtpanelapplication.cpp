/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "lxqtpanelapplication.h"
#include "lxqtpanel.h"
#include <LXQt/Settings>
#include <QtDebug>
#include <QUuid>
#include <X11/Xlib.h>


LxQtPanelApplication::LxQtPanelApplication(int& argc, char** argv, const QString &configFile)
    : LxQt::Application(argc, argv)
{
    if (configFile.isEmpty())
        mSettings = new LxQt::Settings("panel", this);
    else
        mSettings = new LxQt::Settings(configFile, QSettings::IniFormat, this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
		qApp->installNativeEventFilter(this);
#endif

    QStringList panels = mSettings->value("panels").toStringList();

    if (panels.isEmpty())
    {
        panels << "panel1";
    }

    Q_FOREACH(QString i, panels)
    {
        addPanel(i);
    }
}


LxQtPanelApplication::~LxQtPanelApplication()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qApp->removeNativeEventFilter(this);
#endif
    qDeleteAll(mPanels);
}

void LxQtPanelApplication::addNewPanel()
{
    QString name("panel_" + QUuid::createUuid().toString());
    addPanel(name);
    
    QStringList panels = mSettings->value("panels").toStringList();
    panels << name;
    mSettings->setValue("panels", panels);
}

void LxQtPanelApplication::addPanel(const QString &name)
{
    LxQtPanel *panel = new LxQtPanel(name);
    mPanels << panel;
    connect(panel, SIGNAL(deletedByUser(LxQtPanel*)),
            this, SLOT(removePanel(LxQtPanel*)));
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

// Qt5 uses native event filter
bool LxQtPanelApplication::nativeEventFilter(const QByteArray & eventType, void * message, long * result)
{
    if(eventType != "xcb_generic_event_t") // We only want to handle XCB events
        return false;
    xcb_generic_event_t* event = reinterpret_cast<xcb_generic_event_t*>(message);
    foreach(LxQtPanel *i, mPanels)
        i->x11EventFilter(event);
    return false;
}

#else

// This X11 event is no longer supported in Qt5
bool LxQtPanelApplication::x11EventFilter(XEvent * event)
{
    foreach(LxQtPanel *i, mPanels)
        i->x11EventFilter(event);
    return false;
}

#endif // Qt5

void LxQtPanelApplication::removePanel(LxQtPanel* panel)
{
    Q_ASSERT(mPanels.contains(panel));

    mPanels.removeAll(panel);
    
    QStringList panels = mSettings->value("panels").toStringList();
    panels.removeAll(panel->name());
    mSettings->setValue("panels", panels);
    
    panel->deleteLater();
}
