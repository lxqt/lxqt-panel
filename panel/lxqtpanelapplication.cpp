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
#include "config/configpaneldialog.h"
#include <LXQt/Settings>
#include <QtDebug>
#include <QUuid>
#include <QScreen>
#include <QWindow>

LxQtPanelApplication::LxQtPanelApplication(int& argc, char** argv, const QString &configFile)
    : LxQt::Application(argc, argv)
{
    if (configFile.isEmpty())
        mSettings = new LxQt::Settings("panel", this);
    else
        mSettings = new LxQt::Settings(configFile, QSettings::IniFormat, this);

    // This is a workaround for Qt 5 bug #40681.
    Q_FOREACH(QScreen* screen, screens())
    {
        connect(screen, &QScreen::destroyed, this, &LxQtPanelApplication::screenDestroyed);
    }
    connect(this, &QGuiApplication::screenAdded, this, &LxQtPanelApplication::handleScreenAdded);

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
    qDeleteAll(mPanels);
}

void LxQtPanelApplication::addNewPanel()
{
    QString name("panel_" + QUuid::createUuid().toString());
    LxQtPanel *p = addPanel(name);
    QStringList panels = mSettings->value("panels").toStringList();
    panels << name;
    mSettings->setValue("panels", panels);

    // Poupup the configuration dialog to allow user configuration right away
    ConfigPanelDialog::exec(p);
}

LxQtPanel* LxQtPanelApplication::addPanel(const QString& name)
{
    LxQtPanel *panel = new LxQtPanel(name);
    mPanels << panel;
    connect(panel, SIGNAL(deletedByUser(LxQtPanel*)),
            this, SLOT(removePanel(LxQtPanel*)));
    return panel;
}

void LxQtPanelApplication::handleScreenAdded(QScreen* newScreen)
{
    // qDebug() << "LxQtPanelApplication::handleScreenAdded" << newScreen;
    connect(newScreen, &QScreen::destroyed, this, &LxQtPanelApplication::screenDestroyed);
}

void LxQtPanelApplication::reloadPanelsAsNeeded()
{
    // NOTE by PCMan: This is a workaround for Qt 5 bug #40681.
    // Here we try to re-create the missing panels which are deleted in
    // LxQtPanelApplication::screenDestroyed().

    // qDebug() << "LxQtPanelApplication::reloadPanelsAsNeeded()";
    QStringList names = mSettings->value("panels").toStringList();
    Q_FOREACH(const QString& name, names)
    {
        bool found = false;
        Q_FOREACH(LxQtPanel* panel, mPanels)
        {
            if(panel->name() == name)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            // the panel is found in the config file but does not exist, create it.
            qDebug() << "Workaround Qt 5 bug #40681: re-create panel:" << name;
            addPanel(name);
        }
    }
    qApp->setQuitOnLastWindowClosed(true);
}

void LxQtPanelApplication::screenDestroyed(QObject* screenObj)
{
    // NOTE by PCMan: This is a workaround for Qt 5 bug #40681.
    // With this very dirty workaround, we can fix lxde/lxde-qt bug #204, #205, and #206.
    // Qt 5 has two new regression bugs which breaks lxqt-panel in a multihead environment.
    // #40681: Regression bug: QWidget::winId() returns old value and QEvent::WinIdChange event is not emitted sometimes. (multihead setup)
    // #40791: Regression: QPlatformWindow, QWindow, and QWidget::winId() are out of sync.
    // Explanations for the workaround:
    // Internally, Qt mantains a list of QScreens and update it when XRandR configuration changes.
    // When the user turn off an monitor with xrandr --output <xxx> --off, this will destroy the QScreen
    // object which represent the output. If the QScreen being destroyed contains our panel widget,
    // Qt will call QWindow::setScreen(0) on the internal windowHandle() of our panel widget to move it
    // to the primary screen. However, moving a window to a different screen is more than just changing
    // its position. With XRandR, all screens are actually part of the same virtual desktop. However,
    // this is not the case in other setups, such as Xinerama and moving a window to another screen is
    // not possible unless you destroy the widget and create it again for a new screen.
    // Therefore, Qt destroy the widget and re-create it when moving our panel to a new screen.
    // Unfortunately, destroying the window also destroy the child windows embedded into it,
    // using XEMBED such as the tray icons. (#206)
    // Second, when the window is re-created, the winId of the QWidget is changed, but Qt failed to
    // generate QEvent::WinIdChange event so we have no way to know that. We have to set
    // some X11 window properties using the native winId() to make it a dock, but this stop working
    // because we cannot get the correct winId(), so this causes #204 and #205.
    //
    // The workaround is very simple. Just completely destroy the panel before Qt has a chance to do
    // QWindow::setScreen() for it. Later, we reload the panel ourselves. So this can bypassing the Qt bugs.
    QScreen* screen = static_cast<QScreen*>(screenObj);
    bool reloadNeeded = false;
    qApp->setQuitOnLastWindowClosed(false);
    Q_FOREACH(LxQtPanel* panel, mPanels)
    {
        QWindow* panelWindow = panel->windowHandle();
        if(panelWindow && panelWindow->screen() == screen)
        {
            // the screen containing the panel is destroyed
            // delete and then re-create the panel ourselves
            QString name = panel->name();
            panel->saveSettings(false);
            delete panel; // delete the panel, so Qt does not have a chance to set a new screen to it.
            mPanels.removeAll(panel);
            reloadNeeded = true;
            qDebug() << "Workaround Qt 5 bug #40681: delete panel:" << name;
        }
    }
    if(reloadNeeded)
        QTimer::singleShot(1000, this, SLOT(reloadPanelsAsNeeded()));
    else
        qApp->setQuitOnLastWindowClosed(true);
}

void LxQtPanelApplication::removePanel(LxQtPanel* panel)
{
    Q_ASSERT(mPanels.contains(panel));

    mPanels.removeAll(panel);

    QStringList panels = mSettings->value("panels").toStringList();
    panels.removeAll(panel->name());
    mSettings->setValue("panels", panels);

    panel->deleteLater();
}
