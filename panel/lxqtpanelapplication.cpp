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


static inline QList<QByteArray> detectDesktopEnvironment()
{
    const QByteArray xdgCurrentDesktop = qgetenv("XDG_CURRENT_DESKTOP");
    if (!xdgCurrentDesktop.isEmpty())
    {
        // KDE, GNOME, UNITY, LXDE, MATE, XFCE...
        // But also LXQt:$COMPOSITOR:wlroots
        QList<QByteArray> list = xdgCurrentDesktop.toUpper().split(':');
        if(!list.isEmpty())
        {
            if(list.first() == QByteArrayLiteral("LXQT"))
                list.removeFirst();
            if(!list.isEmpty())
                return list;
        }
    }

    // Classic fallbacks
    if (!qEnvironmentVariableIsEmpty("KDE_FULL_SESSION"))
        return {QByteArrayLiteral("KDE")};

    // Fallback to checking $DESKTOP_SESSION (unreliable)
    QByteArray desktopSession = qgetenv("DESKTOP_SESSION");

    // This can be a path in /usr/share/xsessions
    int slash = desktopSession.lastIndexOf('/');
    // try decoding just the basename
    desktopSession = desktopSession.mid(slash + 1);

    if (desktopSession == "kde" || desktopSession == "plasma")
        return {QByteArrayLiteral("KDE")};

    return {};
}

QString findBestBackend()
{
    QStringList dirs;

    // LXQTPANEL_PLUGIN_PATH is not always defined, skip if empty
    QStringList pluginPaths = QProcessEnvironment::systemEnvironment()
                                  .value(QStringLiteral("LXQTPANEL_PLUGIN_PATH"))
                                  .split(QStringLiteral(":"), Qt::SkipEmptyParts);
    if(!pluginPaths.isEmpty())
        dirs << pluginPaths;

    dirs << QStringLiteral(PLUGIN_DIR);

    QString lastBackendFile;
    int lastBackendScore = 0;

    QList<QByteArray> desktops = detectDesktopEnvironment();
    for(const QByteArray& desktop : desktops)
    {
        QString key = QString::fromUtf8(desktop);

        for(const QString& dir : std::as_const(dirs))
        {
            QDir backendsDir(dir);
            backendsDir.cd(QLatin1String("backend"));

            const auto entryList = backendsDir.entryList(QDir::Files);
            for(const QString& fileName : entryList)
            {
                const QString absPath = backendsDir.absoluteFilePath(fileName);
                QPluginLoader loader(absPath);
                loader.load();
                if(!loader.isLoaded())
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
                    int score = backend->getBackendScore(key);
                    if(score > lastBackendScore)
                    {
                        lastBackendFile = absPath;
                        lastBackendScore = score;
                    }
                }
                loader.unload();
            }
        }

        // Double the score before going to next key
        lastBackendScore *= 2;
    }

    if(lastBackendScore == 0)
        return QString(); // No available backend is good for this environment

    return lastBackendFile;
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
    QPluginLoader loader;

    // First try to load user preferred backend
    QString preferredBackend = mSettings->value(QStringLiteral("preferred_backend")).toString();
    if(!preferredBackend.isEmpty())
    {
        loader.setFileName(preferredBackend);
        loader.load();

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

    if(!mWMBackend)
    {
        // If user prefferred is not valid, find best available backend
        QString fileName = findBestBackend();

        if(!fileName.isEmpty())
        {
            loader.setFileName(fileName);
            loader.load();

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

    // This is a workaround for Qt 5 bug #40681.
    const auto allScreens = screens();
    for(QScreen* screen : allScreens)
    {
        connect(screen, &QScreen::destroyed, this, &LXQtPanelApplication::screenDestroyed);
    }
    connect(this, &QGuiApplication::screenAdded, this, &LXQtPanelApplication::handleScreenAdded);
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

    for(const QString& i : std::as_const(panels))
    {
        addPanel(i);
    }
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
