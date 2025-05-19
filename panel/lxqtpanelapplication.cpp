/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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
#include "lxqtpanelapplication_p.h"

#include "config/configpaneldialog.h"
#include "lxqtpanel.h"

#include <QCommandLineParser>
#include <QScreen>
#include <QUuid>
#include <QWindow>
#include <QtDebug>
#include <LXQt/Settings>

#include <QPluginLoader>
#include <QDir>
#include <QProcessEnvironment>

#include "backends/lxqtdummywmbackend.h"

static inline QMap<QString, int> getBackendScoreMap( QString compositor )
{
    QStringList dirs;
    dirs << QProcessEnvironment::systemEnvironment().value(QStringLiteral("LXQTPANEL_PLUGIN_PATH")).split(QStringLiteral(":"));
    dirs << QStringLiteral(PLUGIN_DIR);

    QMap<QString, int> backendScoreMap;

    for(const QString& dir : std::as_const(dirs))
    {
        QDir backendsDir(dir);
        if ( QFile::exists( dir + QStringLiteral("/backend") ) ) {
            backendsDir.cd(QLatin1String("backend"));
        }

        const auto entryList = backendsDir.entryInfoList(QStringList() << QStringLiteral("*.so"), QDir::Files|QDir::System|QDir::Readable);
        for(QFileInfo info: entryList)
        {
            QPluginLoader loader(info.absoluteFilePath());
            if(!loader.load())
            {
                QString err = loader.errorString();
                qWarning() << "Backend error:" << err;
            }

            QObject *plugin = loader.instance();
            if(!plugin)
                continue;

            ILXQtWMBackendLibrary *backend = qobject_cast<ILXQtWMBackendLibrary *>(plugin);
            if(backend)
            {
                backendScoreMap[ info.fileName() ] = backend->getBackendScore( compositor );
            }
            loader.unload();
        }
    }

    return backendScoreMap;
}

static inline QString getBackendFilePath( QString name )
{
    // If we do not have a full library name, line lib_labwc_backend.so,
    // then build a name based on default heuristic: libwmbackend_<name>.so
    if (!name.startsWith(QStringLiteral("lib")) || !name.endsWith(QStringLiteral(".so")))
    {
        if ( !name.startsWith( QStringLiteral("libwmbackend_") ) )
        {
            name = QString( QStringLiteral("libwmbackend_%1") ).arg( name );
        }
        if ( !name.endsWith( QStringLiteral(".so") ) )
        {
            name = QString( QStringLiteral("%1.so") ).arg( name );
        }
    }

    QStringList dirs;
    dirs << QProcessEnvironment::systemEnvironment().value(QStringLiteral("LXQTPANEL_PLUGIN_PATH")).split(QStringLiteral(":"));
    dirs << QStringLiteral(PLUGIN_DIR);

    QMap<QString, int> backendScoreMap;

    for(const QString& dir : std::as_const(dirs))
    {
        QDir backendsDir(dir);
        if ( QFile::exists( dir + QStringLiteral("/backend") ) ) {
            backendsDir.cd(QLatin1String("backend"));
        }

        if ( backendsDir.exists( name ) )
        {
            return backendsDir.absoluteFilePath( name );
        }
    }

    return QString();
}

static inline bool testBackend( QString backendName )
{
    QString backendPath = getBackendFilePath( backendName );

    QPluginLoader loader(backendPath);
    if(!loader.load())
    {
        qWarning() << "Backend error:" << loader.errorString();
        return false;
    }

    QObject *plugin = loader.instance();
    if(!plugin) {
        qWarning() << "Failed to create the plugin instance";
        return false;
    }

    ILXQtWMBackendLibrary *backend = qobject_cast<ILXQtWMBackendLibrary *>(plugin);
    bool okay = false;
    if(backend)
    {
        okay = true;
    }

    loader.unload();

    return okay;
}

LXQtPanelApplicationPrivate::LXQtPanelApplicationPrivate(LXQtPanelApplication *q)
    : mSettings(nullptr)
    , mWMBackend(nullptr)
    , q_ptr(q)
{

}


ILXQtPanel::Position LXQtPanelApplicationPrivate::computeNewPanelPosition(const LXQtPanel *p, const int screenNum)
{
    Q_Q(LXQtPanelApplication);
    QList<bool> screenPositions(4, false); // false means not occupied

    for (int i = 0; i < q->mPanels.size(); ++i) {
        if (p != q->mPanels.at(i)) {
            // We are not the newly added one
            if (screenNum == q->mPanels.at(i)->screenNum()) { // Panels on the same screen
                int p = static_cast<int> (q->mPanels.at(i)->position());
                screenPositions[p] = true; // occupied
            }
        }
    }

    int availablePosition = 0;

    for (int i = 0; i < 4; ++i) { // Bottom, Top, Left, Right
        if (!screenPositions[i]) {
            availablePosition = i;
            break;
        }
    }

    return static_cast<ILXQtPanel::Position> (availablePosition);
}

void LXQtPanelApplicationPrivate::loadBackend()
{
    /**
     * 1. Get the XDG_CURRENT_DESKTOP. It's a colon separate list.
     * 2. Get the preferredBackend. It's a comma separated list.
     * 3. First attempt to match some value in XDG_CURRENT_DESKTOP with any value in preferredBackend.
     * 4. If it matches, end of story. Else, we attempt to deduce the backend based on XDG_CURRENT_DESKTOP:
     *    a. X11 -> xcb
     *    b. kwin_wayland -> plasma
     *    c. wayfire -> wayfire
     *    d. wayland -> wlroots
     *    e. other -> dummy
     */

    // Get and split XDG_CURRENT_DESKTOP.
    QStringList xdgCurrentDesktops = qEnvironmentVariable( "XDG_CURRENT_DESKTOP" ).split( QStringLiteral(":") );

    // Get and split XDG_SESSION_TYPE.
    QString xdgSessionType = qEnvironmentVariable( "XDG_SESSION_TYPE" );

    // Get the preferred backends
    QStringList preferredBackends = mSettings->value(QStringLiteral("preferred_backend")).toStringList();

    // The preferred backend
    QString preferredBackend;

	for ( QString xdgCurrentDesktop: xdgCurrentDesktops )
	{
		for ( QString backend: preferredBackends )
		{
			QStringList parts = backend.split(QStringLiteral(":"));
            // Invalid format
            if (parts.count() != 2)
            {
                continue;
            }

			if (parts[0].compare(xdgCurrentDesktop, Qt::CaseInsensitive) == 0 && testBackend(parts[1]))
			{
				preferredBackend = parts[1];
				break;
			}
		}
	}

    /** No special considerations. Attempt auto-detection of the platform */
    if ( preferredBackend.isEmpty() ) {
        qDebug() << "No user preferences available. Attempting auto-detection.";

        // It's XCB/X11
        if ( xdgSessionType == QStringLiteral("x11") ) {
            preferredBackend = QStringLiteral("xcb");
        }

        // It's wayland
        else {
            int bestScore = 0;
            for ( QString xdgCurrentDesktop: xdgCurrentDesktops )
        	{
                QMap<QString, int> backendScoreMap = getBackendScoreMap( xdgCurrentDesktop );
                for( QString backend: backendScoreMap.keys() )
                {
                    if ( backendScoreMap[ backend ] > bestScore )
                    {
                        bestScore = backendScoreMap[ backend ];
                        // No need to call testBackend().
                        // We can be sure the plugin can be loaded.
                        // Because we have a score.
                        preferredBackend = backend;
                    }
                }
            }
        }
    }

    if (preferredBackend.isEmpty() && xdgCurrentDesktops.contains(QStringLiteral("wlroots"), Qt::CaseInsensitive))
    {
        qDebug() << "Specialized backend unavailable. Falling back to generic wlroots";
        preferredBackend = QStringLiteral("wlroots");
    }

    QPluginLoader loader;

    // We now have the preferred backend.
    // We have taken into consideration, the user's choice.
    // In case it was unavailable, a default one has been chosen.
    if(!preferredBackend.isEmpty())
    {
        loader.setFileName(getBackendFilePath(preferredBackend));
        if (loader.load())
        {
            QObject *plugin = loader.instance();
            ILXQtWMBackendLibrary *backend = qobject_cast<ILXQtWMBackendLibrary *>(plugin);
            if(backend)
            {
                mWMBackend = backend->instance();
            }
            else
            {
                // Plugin not valid
                loader.unload();
            }
        }
        else
        {
            qWarning() << loader.errorString();
        }
    }

    if(mWMBackend)
    {
        qDebug() << "\nPanel backend:" << preferredBackend << "\n";
    }
    else
    {
        // If no backend can be found fall back to dummy backend
        loader.unload();
        mWMBackend = new LXQtDummyWMBackend;

        qWarning() << "\n"
                   << "ERROR: Could not create a backend for window managment operations.\n"
                   << "Falling back to dummy backend. Some functions will not be available.\n"
                   << "\n";
    }

    mWMBackend->setParent(q_ptr);
}

LXQtPanelApplication::LXQtPanelApplication(int& argc, char** argv)
    : LXQt::Application(argc, argv, true),
    d_ptr(new LXQtPanelApplicationPrivate(this))

{
    Q_D(LXQtPanelApplication);

    QCoreApplication::setApplicationName(QLatin1String("lxqt-panel"));
    const QString VERINFO = QStringLiteral(LXQT_PANEL_VERSION
                                           "\nliblxqt   " LXQT_VERSION
                                           "\nQt        " QT_VERSION_STR);

    QCoreApplication::setApplicationVersion(VERINFO);

    QCommandLineParser parser;
    parser.setApplicationDescription(QLatin1String("LXQt Panel"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption configFileOption(QStringList()
            << QLatin1String("c") << QLatin1String("config") << QLatin1String("configfile"),
            QCoreApplication::translate("main", "Use alternate configuration file."),
            QCoreApplication::translate("main", "Configuration file"));
    parser.addOption(configFileOption);

    parser.process(*this);

    const QString configFile = parser.value(configFileOption);

    if (configFile.isEmpty())
        d->mSettings = new LXQt::Settings(QLatin1String("panel"), this);
    else
        d->mSettings = new LXQt::Settings(configFile, QSettings::IniFormat, this);

    d->loadBackend();

    const auto allScreens = screens();

    if (QGuiApplication::platformName() != QStringLiteral("wayland"))
    {
        // This is a workaround for Qt 5 bug #40681.
        for(QScreen* screen : allScreens)
        {
            connect(screen, &QScreen::destroyed, this, &LXQtPanelApplication::screenDestroyed);
        }
        connect(this, &QGuiApplication::screenAdded, this, &LXQtPanelApplication::handleScreenAdded);
    }
    else
    {
        connect(this, &QGuiApplication::screenAdded, this, &LXQtPanelApplication::handleWaylandScreenAdded);
    }

    connect(this, &QCoreApplication::aboutToQuit, this, &LXQtPanelApplication::cleanup);


    QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();

    // WARNING: Giving a separate icon theme to the panel is wrong and has side effects.
    // However, it is optional and can be used as the last resort for avoiding a low
    // contrast in the case of symbolic SVG icons. (The correct way of doing that is
    // using a Qt widget style that can assign a separate theme/QPalette to the panel.)
    mGlobalIconTheme = QIcon::themeName();
    const QString iconTheme = d->mSettings->value(QStringLiteral("iconTheme")).toString();
    if (!iconTheme.isEmpty())
        QIcon::setThemeName(iconTheme);

    if (panels.isEmpty())
    {
        panels << QStringLiteral("panel1");
    }

    for (const QString& i : std::as_const(panels))
    {
        if (QGuiApplication::platformName() == QStringLiteral("wayland"))
        {
            // On Wayland, add a panel that has screen name only if its screen exists.
            bool found = false;
            d->mSettings->beginGroup(i);
            auto screenName = d->mSettings->value(QStringLiteral(CFG_KEY_SCREENNAME)).toString();
            d->mSettings->endGroup();
            if (screenName.isEmpty())
                found = true; // add the panel, anyway
            else
            {
                for (const auto& screen : allScreens)
                {
                    if (screen->name() == screenName)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
                continue;
        }

        addPanel(i);
    }
    // if no panel can be added on Wayland, forcefully add the first one
    if (mPanels.isEmpty())
        addPanel(panels.at(0));
}

LXQtPanelApplication::~LXQtPanelApplication()
{
    delete d_ptr;
}

void LXQtPanelApplication::cleanup()
{
    qDeleteAll(mPanels);
}

void LXQtPanelApplication::addNewPanel()
{
    Q_D(LXQtPanelApplication);

    QString name(QStringLiteral("panel_") + QUuid::createUuid().toString());

    LXQtPanel *p = addPanel(name);
    int screenNum = p->screenNum();
    ILXQtPanel::Position newPanelPosition = d->computeNewPanelPosition(p, screenNum);
    p->setPosition(screenNum, newPanelPosition, true);
    QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
    panels << name;
    d->mSettings->setValue(QStringLiteral("panels"), panels);

    // Poupup the configuration dialog to allow user configuration right away
    p->showConfigDialog();
}

LXQtPanel* LXQtPanelApplication::addPanel(const QString& name)
{
    Q_D(LXQtPanelApplication);

    LXQtPanel *panel = new LXQtPanel(name, d->mSettings);
    mPanels << panel;

    // reemit signals
    connect(panel, &LXQtPanel::deletedByUser, this, &LXQtPanelApplication::removePanel);
    connect(panel, &LXQtPanel::pluginAdded, this, &LXQtPanelApplication::pluginAdded);
    connect(panel, &LXQtPanel::pluginRemoved, this, &LXQtPanelApplication::pluginRemoved);

    return panel;
}

void LXQtPanelApplication::handleScreenAdded(QScreen* newScreen)
{
    // qDebug() << "LXQtPanelApplication::handleScreenAdded" << newScreen;
    connect(newScreen, &QScreen::destroyed, this, &LXQtPanelApplication::screenDestroyed);
}

void LXQtPanelApplication::handleWaylandScreenAdded(QScreen* newScreen)
{
    Q_D(LXQtPanelApplication);

    const QStringList names = d->mSettings->value(QStringLiteral("panels")).toStringList();
    for (const QString& name : names)
    {
        d->mSettings->beginGroup(name);
        auto screenName = d->mSettings->value(QStringLiteral(CFG_KEY_SCREENNAME)).toString();
        d->mSettings->endGroup();
        if (screenName == newScreen->name())
        {
            bool alreadyExists = false;
            for (const auto& panel : std::as_const(mPanels))
            {
                if (panel->name() == name)
                { // the panel already exists (and is hidden)
                    alreadyExists = true;
                    break;
                }
            }
            if (alreadyExists)
                continue;
            addPanel(name);
        }
    }
}

void LXQtPanelApplication::reloadPanelsAsNeeded()
{
    Q_D(LXQtPanelApplication);

    // NOTE by PCMan: This is a workaround for Qt 5 bug #40681.
    // Here we try to re-create the missing panels which are deleted in
    // LXQtPanelApplication::screenDestroyed().

    // qDebug() << "LXQtPanelApplication::reloadPanelsAsNeeded()";
    const QStringList names = d->mSettings->value(QStringLiteral("panels")).toStringList();
    for(const QString& name : names)
    {
        bool found = false;
        for(LXQtPanel* panel : std::as_const(mPanels))
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

void LXQtPanelApplication::screenDestroyed(QObject* screenObj)
{
    // NOTE by PCMan: This is a workaround for Qt 5 bug #40681.
    // With this very dirty workaround, we can fix lxqt/lxqt bug #204, #205, and #206.
    // Qt 5 has two new regression bugs which breaks lxqt-panel in a multihead environment.
    // #40681: Regression bug: QWidget::winId() returns old value and QEvent::WinIdChange event is not emitted sometimes. (multihead setup)
    // #40791: Regression: QPlatformWindow, QWindow, and QWidget::winId() are out of sync.
    // Explanations for the workaround:
    // Internally, Qt maintains a list of QScreens and update it when XRandR configuration changes.
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
    for(LXQtPanel* panel : std::as_const(mPanels))
    {
        QWindow* panelWindow = panel->windowHandle();
        if(panelWindow && panelWindow->screen() == screen)
        {
            // the screen containing the panel is destroyed
            // delete and then re-create the panel ourselves
            QString name = panel->name();
            panel->saveSettings(false);
            mPanels.removeAll(panel);
            delete panel; // delete the panel, so Qt does not have a chance to set a new screen to it.
            reloadNeeded = true;
            qDebug() << "Workaround Qt 5 bug #40681: delete panel:" << name;
        }
    }
    if(reloadNeeded)
        QTimer::singleShot(1000, this, SLOT(reloadPanelsAsNeeded()));
    else
        qApp->setQuitOnLastWindowClosed(true);
}

void LXQtPanelApplication::removePanel(LXQtPanel* panel)
{
    Q_D(LXQtPanelApplication);
    Q_ASSERT(mPanels.contains(panel));

    mPanels.removeAll(panel);

    QStringList panels = d->mSettings->value(QStringLiteral("panels")).toStringList();
    panels.removeAll(panel->name());
    d->mSettings->setValue(QStringLiteral("panels"), panels);

    panel->deleteLater();
}

bool LXQtPanelApplication::isPluginSingletonAndRunning(QString const & pluginId) const
{
    for (auto const & panel : mPanels)
        if (panel->isPluginSingletonAndRunning(pluginId))
            return true;

    return false;
}

ILXQtAbstractWMInterface *LXQtPanelApplication::getWMBackend() const
{
    Q_D(const LXQtPanelApplication);
    return d->mWMBackend;
}

// See LXQtPanelApplication::LXQtPanelApplication for why this isn't good.
void LXQtPanelApplication::setIconTheme(const QString &iconTheme)
{
    Q_D(LXQtPanelApplication);

    d->mSettings->setValue(QStringLiteral("iconTheme"), iconTheme == mGlobalIconTheme ? QString() : iconTheme);
    QString newTheme = iconTheme.isEmpty() ? mGlobalIconTheme : iconTheme;
    if (newTheme != QIcon::themeName())
    {
        QIcon::setThemeName(newTheme);
        for(LXQtPanel* panel : std::as_const(mPanels))
        {
            panel->update();
            panel->updateConfigDialog();
        }
    }
}
