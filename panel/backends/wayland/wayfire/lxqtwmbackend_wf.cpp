#include "wayfire-common.h"
#include "lxqtwmbackend_wf.h"

#include <QIcon>
#include <QTime>
#include <QScreen>
#include <algorithm>

QString U8Str(const char *str)
{
    return QString::fromUtf8(str);
}

static inline QString getPixmapIcon(QString name)
{
    QStringList paths{
        U8Str("/usr/local/share/pixmaps/"),
        U8Str("/usr/share/pixmaps/"),
    };

    QStringList sfxs{
        U8Str(".svg"), U8Str(".png"), U8Str(".xpm")
    };

    for (QString path : paths)
    {
        for (QString sfx : sfxs)
        {
            if (QFile::exists(path + name + sfx))
            {
                return path + name + sfx;
            }
        }
    }

    return QString();
}

QIcon getIconForAppId(QString mAppId)
{
    if (mAppId.isEmpty() or (mAppId == U8Str("Unknown")))
    {
        return QIcon();
    }

    /** Wine apps */
    if (mAppId.endsWith(U8Str(".exe")))
    {
        return QIcon::fromTheme(U8Str("wine"));
    }

    /** Check if a theme icon exists called @mAppId */
    if (QIcon::hasThemeIcon(mAppId))
    {
        return QIcon::fromTheme(mAppId);
    }
    /** Check if the theme icon is @mAppId, but all lower-case letters */
    else if (QIcon::hasThemeIcon(mAppId.toLower()))
    {
        return QIcon::fromTheme(mAppId.toLower());
    }

    QStringList appDirs = {
        QDir::home().filePath(U8Str(".local/share/applications/")),
        U8Str("/usr/local/share/applications/"),
        U8Str("/usr/share/applications/"),
    };

    /**
     * Assume mAppId == desktop-file-name (ideal situation) or mAppId.toLower() == desktop-file-name (cheap
     * fallback)
     */
    QString iconName;

    for (QString path : appDirs)
    {
        /** Get the icon name from desktop (mAppId: as it is) */
        if (QFile::exists(path + mAppId + U8Str(".desktop")))
        {
            QSettings desktop(path + mAppId + U8Str(".desktop"), QSettings::IniFormat);
            iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
        }
        /** Get the icon name from desktop (mAppId: all lower-case letters) */
        else if (QFile::exists(path + mAppId.toLower() + U8Str(".desktop")))
        {
            QSettings desktop(path + mAppId.toLower() + U8Str(".desktop"), QSettings::IniFormat);
            iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
        }

        /** No icon specified: try else-where */
        if (iconName.isEmpty())
        {
            continue;
        }

        /** We got an iconName, and it's in the current theme */
        if (QIcon::hasThemeIcon(iconName))
        {
            return QIcon::fromTheme(iconName);
        }
        /** Not a theme icon, but an absolute path */
        else if (QFile::exists(iconName))
        {
            return QIcon(iconName);
        }
        /** Not theme icon or absolute path. So check /usr/share/pixmaps/ */
        else
        {
            iconName = getPixmapIcon(iconName);

            if (not iconName.isEmpty())
            {
                return QIcon(iconName);
            }
        }
    }

    /* Check all desktop files for @mAppId */
    for (QString path : appDirs)
    {
        QStringList desktops = QDir(path).entryList({U8Str("*.desktop")});
        for (QString dskf : desktops)
        {
            QSettings desktop(path + dskf, QSettings::IniFormat);

            QString exec = desktop.value(U8Str("Desktop Entry/Exec"), U8Str("abcd1234/-")).toString();
            QString name = desktop.value(U8Str("Desktop Entry/Name"), U8Str("abcd1234/-")).toString();
            QString cls  = desktop.value(U8Str("Desktop Entry/StartupWMClass"),
                U8Str("abcd1234/-")).toString();

            QString execPath = U8Str(std::filesystem::path(exec.toStdString()).filename().c_str());

            if (mAppId.compare(execPath, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            } else if (mAppId.compare(name, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            } else if (mAppId.compare(cls, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            }

            if (not iconName.isEmpty())
            {
                if (QIcon::hasThemeIcon(iconName))
                {
                    return QIcon::fromTheme(iconName);
                } else if (QFile::exists(iconName))
                {
                    return QIcon(iconName);
                } else
                {
                    iconName = getPixmapIcon(iconName);

                    if (not iconName.isEmpty())
                    {
                        return QIcon(iconName);
                    }
                }
            }
        }
    }

    iconName = getPixmapIcon(iconName);

    if (not iconName.isEmpty())
    {
        return QIcon(iconName);
    }

    return QIcon();
}

LXQtTaskbarWayfireBackend::LXQtTaskbarWayfireBackend(QObject *parent) :
    ILXQtAbstractWMInterface(parent)
{
    mWayfire.reset(new LXQt::Panel::Wayfire());

    // emit workspaceCountChanged()
    connect(mWayfire.get(), &LXQt::Panel::Wayfire::workspaceSetChanged, [this] ( QJsonDocument )
    {
        // QJsonObject response = respJson.object();

        // QJsonObject wsInfo = response[QStringLiteral("new-wset-data")].toObject();
        // QJsonObject output = response[QStringLiteral("output-data")].toObject();

        // if (output[QStringLiteral("id")].toInt() == mScreenId)
        // {
        // mWSetId = wsInfo[QStringLiteral("index")].toInt();

        // QJsonObject ws = wsInfo[QStringLiteral("workspace")].toObject();
        // mWS.row    = ws[QStringLiteral("y")].toInt();
        // mWS.column = ws[QStringLiteral("x")].toInt();

        // if ((bool)tasksSett->value(QStringLiteral("ShowCurrentWSetOnly")) == true)
        // {
        // clearLayout();
        // populateLayout();
        // }
        // }
    });

    // emit currentWorkspaceChanged(idx)
    connect(mWayfire.get(), &LXQt::Panel::Wayfire::workspaceChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();

        QJsonObject wsInfo = response[QStringLiteral("new-workspace")].toObject();

        QJsonObject output    = response[QStringLiteral("output-data")].toObject();
        QString outputName    = output[QStringLiteral("name")].toString();
        QJsonObject workspace = output[QStringLiteral("workspace")].toObject();

        int64_t nRows = workspace[QStringLiteral("grid_height")].toInt();
        int64_t nCols = workspace[QStringLiteral("grid_width")].toInt();

        int64_t row    = wsInfo[QStringLiteral("y")].toInt();
        int64_t column = wsInfo[QStringLiteral("x")].toInt();

        emit currentWorkspaceChanged(row * nRows + column + 1, outputName);
    });

    // emit windowAdded(WId)
    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewMapped, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();

        QJsonObject view = response[QStringLiteral("view")].toObject();

        /** Ghost view: these are unmapped xwayland views */
        if (view[QStringLiteral("pid")].toInt() == -1)
        {
            return;
        }

        /** We want only the "toplevel" views */
        QString role = view[QStringLiteral("role")].toString();

        if (role != QStringLiteral("toplevel"))
        {
            return;
        }

        emit windowAdded(view[QStringLiteral("id")].toInt());
    });

    // emit windowPropertyChanged(WId, state) for all windows
    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewFocused, [this] ( QJsonDocument )
    {
        for ( WaylandId viewId : mViews.keys())
        {
            emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::State);
        }

        emit activeWindowChanged(mWayfire->getActiveView());
    });

    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewTitleChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QStringLiteral("view")].toObject();

        if (view.empty())
        {
            return;
        }

        int64_t viewId = view[QStringLiteral("id")].toInt();

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Title);
    });

    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewAppIdChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QStringLiteral("view")].toObject();

        if (view.empty())
        {
            return;
        }

        int64_t viewId = view[QStringLiteral("id")].toInt();

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::WindowClass);
        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Icon);
    });

    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewOutputChanged, [this] ( QJsonDocument )
    {
        // no-op
    });

    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewWorkspaceChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QStringLiteral("view")].toObject();

        if (view.empty())
        {
            return;
        }

        int64_t viewId = view[QStringLiteral("id")].toInt();

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Workspace);
    });

    connect(mWayfire.get(), &LXQt::Panel::Wayfire::viewUnmapped, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QStringLiteral("view")].toObject();

        if (view.empty())
        {
            return;
        }

        int64_t viewId = view[QStringLiteral("id")].toInt();

        emit windowRemoved(viewId);
    });

    mWayfire->connectToServer();
}

bool LXQtTaskbarWayfireBackend::supportsAction(WId, LXQtTaskBarBackendAction action) const
{
    switch (action)
    {
      case LXQtTaskBarBackendAction::Move:
        return true;

      case LXQtTaskBarBackendAction::Resize:
        return true;

      case LXQtTaskBarBackendAction::Maximize:
        return true;

      case LXQtTaskBarBackendAction::MaximizeVertically:
        return true;

      case LXQtTaskBarBackendAction::MaximizeHorizontally:
        return true;

      case LXQtTaskBarBackendAction::Minimize:
        return true;

      case LXQtTaskBarBackendAction::RollUp:
        return false;

      case LXQtTaskBarBackendAction::FullScreen:
        return true;

      case LXQtTaskBarBackendAction::DesktopSwitch:
        return true;

      case LXQtTaskBarBackendAction::MoveToDesktop:
        return true;

      case LXQtTaskBarBackendAction::MoveToLayer:
        return true;

      case LXQtTaskBarBackendAction::MoveToOutput:
        return true;

      default:
        return false;
    }

    return false;
}

bool LXQtTaskbarWayfireBackend::reloadWindows()
{
    // Force removal and re-adding
    for (WId windowId : mViews.keys())
    {
        emit windowRemoved(windowId);
    }

    QJsonArray views = mWayfire->listViews();
    while (views.count())
    {
        QJsonObject view = views.takeAt(0).toObject();
        WaylandId id(view[QStringLiteral("id")].toInt());

        mViews[id] = view;
    }

    for (WId windowId : mViews.keys())
    {
        emit windowAdded(windowId);
    }

    return true;
}

QVector<WId> LXQtTaskbarWayfireBackend::getCurrentWindows() const
{
    QVector<WId> ids;
    for ( WaylandId viewId : mViews.keys())
    {
        ids << viewId;
    }

    return ids;
}

QString LXQtTaskbarWayfireBackend::getWindowTitle(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return QString();
    }

    return mViews[viewId][QStringLiteral("title")].toString();
}

bool LXQtTaskbarWayfireBackend::applicationDemandsAttention(WId) const
{
    return false;
}

QIcon LXQtTaskbarWayfireBackend::getApplicationIcon(WId windowId, int devicePixels) const
{
    Q_UNUSED(devicePixels)

    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return QIcon();
    }

    return getIconForAppId(mViews[viewId][QStringLiteral("app-id")].toString());
}

QString LXQtTaskbarWayfireBackend::getWindowClass(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return QString();
    }

    return mViews[viewId][QStringLiteral("app-id")].toString();
}

LXQtTaskBarWindowLayer LXQtTaskbarWayfireBackend::getWindowLayer(WId) const
{
    return LXQtTaskBarWindowLayer::Normal;
}

bool LXQtTaskbarWayfireBackend::setWindowLayer(WId, LXQtTaskBarWindowLayer)
{
    return false;
}

LXQtTaskBarWindowState LXQtTaskbarWayfireBackend::getWindowState(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return LXQtTaskBarWindowState::Minimized;
    }

    if (mViews[viewId][QStringLiteral("mapped")].toBool())
    {
        return LXQtTaskBarWindowState::Hidden;
    }

    if (mViews[viewId][QStringLiteral("minimized")].toBool())
    {
        return LXQtTaskBarWindowState::Minimized;
    }

    if (mViews[viewId][QStringLiteral("fullscreen")].toBool())
    {
        return LXQtTaskBarWindowState::FullScreen;
    }

    // WLR_EDGE_TOP | WLR_EDGE_BOTTOM | WLR_EDGE_LEFT | WLR_EDGE_RIGHT == 1 | 2 | 4 | 8 == 15
    if (mViews[viewId][QStringLiteral("tiled")].toInt() == 15)
    {
        return LXQtTaskBarWindowState::Maximized;
    }

    // WLR_EDGE_TOP | WLR_EDGE_BOTTOM == 1 | 2 == 3
    if (mViews[viewId][QStringLiteral("tiled")].toInt() == 3)
    {
        return LXQtTaskBarWindowState::MaximizedVertically;
    }

    // WLR_EDGE_LEFT | WLR_EDGE_RIGHT == 4 | 8 == 12
    if (mViews[viewId][QStringLiteral("tiled")].toInt() == 12)
    {
        return LXQtTaskBarWindowState::MaximizedHorizontally;
    }

    return LXQtTaskBarWindowState::Normal;
}

bool LXQtTaskbarWayfireBackend::setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set)
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    switch (state)
    {
      case LXQtTaskBarWindowState::Minimized:
    {
        mWayfire->minimizeView(viewId, set);
        break;
    }

      case LXQtTaskBarWindowState::Maximized:
    {
        mWayfire->maximizeView(viewId, (set ? 15 : 0));
        break;
    }

      case LXQtTaskBarWindowState::MaximizedVertically:
    {
        mWayfire->maximizeView(viewId, (set ? 3 : 0));
        break;
    }

      case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        mWayfire->maximizeView(viewId, (set ? 12 : 0));
        break;
    }

      case LXQtTaskBarWindowState::Normal:
    {
        if (set)
        {
            mWayfire->restoreView(viewId);
        }

        break;
    }

      case LXQtTaskBarWindowState::FullScreen:
    {
        mWayfire->fullscreenView(viewId, set);
        break;
    }

      default:
        return false;
    }

    return true;
}

bool LXQtTaskbarWayfireBackend::isWindowActive(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return (mWayfire->getActiveView() == viewId);
}

bool LXQtTaskbarWayfireBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) // Cannot be done on a generic wlroots-based compositor!

    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return mWayfire->focusView(viewId);
}

bool LXQtTaskbarWayfireBackend::closeWindow(WId windowId)
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return mWayfire->closeView(viewId);
}

WId LXQtTaskbarWayfireBackend::getActiveWindow() const
{
    return mWayfire->getActiveView();
}

int LXQtTaskbarWayfireBackend::getWorkspacesCount() const
{
    QJsonObject wsetsInfo = mWayfire->getWorkspaceSetsInfo().at(0).toObject();
    QJsonObject workspace = wsetsInfo[QStringLiteral("workspace")].toObject();
    int64_t nRows = workspace[QStringLiteral("grid_height")].toInt();
    int64_t nCols = workspace[QStringLiteral("grid_width")].toInt();

    qDebug() << QJsonDocument(wsetsInfo).toJson().data();

    return (nRows * nCols);
}

QString LXQtTaskbarWayfireBackend::getWorkspaceName(int x, QString outputName) const
{
    return mWayfire->getWorkspaceName(x, outputName);
}

int LXQtTaskbarWayfireBackend::getCurrentWorkspace() const
{
    return 1;
}

bool LXQtTaskbarWayfireBackend::setCurrentWorkspace(int x)
{
    return mWayfire->switchToWorkspace(mWayfire->getActiveOutput(), x);
}

int LXQtTaskbarWayfireBackend::getWindowWorkspace(WId) const
{
    return 1;
}

bool LXQtTaskbarWayfireBackend::setWindowOnWorkspace(WId, int)
{
    return true;
}

void LXQtTaskbarWayfireBackend::moveApplicationToPrevNextMonitor(WId viewId, bool nextOp, bool raiseWindow)
{
    Q_UNUSED(viewId)
    Q_UNUSED(nextOp)
    Q_UNUSED(raiseWindow)
    // Depends on the wsets plugin
    // 1. Get the current output id and its active wset-id
    // 2. Get the previous/next output id and its active wset-id, if any.
    // 3. If we have a target wset, move the viewId from current wset to target wset
    // 4. If raiseWindow == true, set view as focused
}

bool LXQtTaskbarWayfireBackend::isWindowOnScreen(QScreen *scrn, WId windowId) const
{
    WaylandId viewId(windowId);
    return mViews[viewId][QStringLiteral("output")] == scrn->name();
}

bool LXQtTaskbarWayfireBackend::setDesktopLayout(Qt::Orientation, int, int, bool)
{
    // Wayfire does not support dynamic setting of desktops.
    return false;
}

void LXQtTaskbarWayfireBackend::moveApplication(WId)
{
    // no-op
}

void LXQtTaskbarWayfireBackend::resizeApplication(WId)
{
    // no-op
}

void LXQtTaskbarWayfireBackend::refreshIconGeometry(WId, const QRect &)
{
    // no-op
}

bool LXQtTaskbarWayfireBackend::isAreaOverlapped(const QRect &) const
{
    return false;
}

bool LXQtTaskbarWayfireBackend::isShowingDesktop() const
{
    return mIsDesktopShowing;
}

bool LXQtTaskbarWayfireBackend::showDesktop(bool yes)
{
    if (mIsDesktopShowing == yes)
    {
        return true;
    }

    mIsDesktopShowing = yes;

    return mWayfire->showDesktop(mWayfire->getActiveOutput());
}

int LXQtWMBackendWayfireLibrary::getBackendScore(const QString& key) const
{
    // Only wayfire is supported
    if ((key == QStringLiteral("wayfire")) || (key == QStringLiteral("Wayfire")))
    {
        return 100;
    }

    // Unsupported
    return 0;
}

ILXQtAbstractWMInterface*LXQtWMBackendWayfireLibrary::instance() const
{
    return new LXQtTaskbarWayfireBackend(nullptr);
}
