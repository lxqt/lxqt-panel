#include "wayfire-common.h"
#include "lxqtwmbackend_wf.h"

#include <QIcon>
#include <QTime>
#include <QScreen>
#include <algorithm>

// Shortforms
#define QSL QStringLiteral
#define U8Str QString::fromUtf8

static inline bool isValidToplevel(QJsonObject view)
{
    if (view.isEmpty())
    {
        return false;
    }

    /** Ghost view: these are unmapped xwayland views */
    if (view[QSL("pid")].toInt() == -1)
    {
        return false;
    }

    /** We want only the "toplevel" views */
    if (view[QSL("role")].toString() != QSL("toplevel"))
    {
        return false;
    }

    return true;
}

static inline QString getPixmapIcon(QString name)
{
    QStringList paths{
        QSL("/usr/local/share/pixmaps/"),
        QSL("/usr/share/pixmaps/"),
    };

    QStringList sfxs{
        QSL(".svg"), QSL(".png"), QSL(".xpm")
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
    if (mAppId.isEmpty() or (mAppId == QSL("Unknown")))
    {
        return QIcon();
    }

    /** Wine apps */
    if (mAppId.endsWith(QSL(".exe")))
    {
        return QIcon::fromTheme(QSL("wine"));
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
        QDir::home().filePath(QSL(".local/share/applications/")),
        QSL("/usr/local/share/applications/"),
        QSL("/usr/share/applications/"),
    };

    /**
     * Assume mAppId == desktop-file-name (ideal situation) or mAppId.toLower() == desktop-file-name (cheap
     * fallback)
     */
    QString iconName;

    for (QString path : appDirs)
    {
        /** Get the icon name from desktop (mAppId: as it is) */
        if (QFile::exists(path + mAppId + QSL(".desktop")))
        {
            QSettings desktop(path + mAppId + QSL(".desktop"), QSettings::IniFormat);
            iconName = desktop.value(QSL("Desktop Entry/Icon")).toString();
        }
        /** Get the icon name from desktop (mAppId: all lower-case letters) */
        else if (QFile::exists(path + mAppId.toLower() + QSL(".desktop")))
        {
            QSettings desktop(path + mAppId.toLower() + QSL(".desktop"), QSettings::IniFormat);
            iconName = desktop.value(QSL("Desktop Entry/Icon")).toString();
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
        QStringList desktops = QDir(path).entryList({QSL("*.desktop")});
        for (QString dskf : desktops)
        {
            QSettings desktop(path + dskf, QSettings::IniFormat);

            QString exec = desktop.value(QSL("Desktop Entry/Exec"), QSL("abcd1234/-")).toString();
            QString name = desktop.value(QSL("Desktop Entry/Name"), QSL("abcd1234/-")).toString();
            QString cls  = desktop.value(QSL("Desktop Entry/StartupWMClass"),
                QSL("abcd1234/-")).toString();

            QString execPath = U8Str(std::filesystem::path(exec.toStdString()).filename().c_str());

            if (mAppId.compare(execPath, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(QSL("Desktop Entry/Icon")).toString();
            } else if (mAppId.compare(name, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(QSL("Desktop Entry/Icon")).toString();
            } else if (mAppId.compare(cls, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(QSL("Desktop Entry/Icon")).toString();
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
    // emit workspaceCountChanged()
    connect(&mWayfire, &LXQt::Panel::Wayfire::workspaceSetChanged, [this] ( QJsonDocument )
    {
        // no-op
    });

    // emit currentWorkspaceChanged(idx)
    connect(&mWayfire, &LXQt::Panel::Wayfire::workspaceChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();

        QJsonObject wsInfo = response[QSL("new-workspace")].toObject();

        QJsonObject output    = response[QSL("output-data")].toObject();
        QString outputName    = output[QSL("name")].toString();
        QJsonObject workspace = output[QSL("workspace")].toObject();

        int64_t nRows = workspace[QSL("grid_height")].toInt();

        int64_t row    = wsInfo[QSL("y")].toInt();
        int64_t column = wsInfo[QSL("x")].toInt();

        emit currentWorkspaceChanged(row * nRows + column + 1, outputName);
    });

    // emit windowAdded(WId)
    connect(&mWayfire, &LXQt::Panel::Wayfire::viewMapped, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();

        QJsonObject view = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());

        if (mViews.contains(viewId))
        {
            emit windowRemoved(viewId);
        }

        mViews[viewId] = view;

        emit windowAdded(view[QSL("id")].toInt());
    });

    // emit windowPropertyChanged(WId, state) for all windows
    connect(&mWayfire, &LXQt::Panel::Wayfire::viewFocused, [this] ( QJsonDocument respJson )
    {
        for ( WaylandId viewId : mViews.keys())
        {
            emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::State);
        }

        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());
        mViews[viewId] = view;

        emit activeWindowChanged(mWayfire.getActiveView());
    });

    connect(&mWayfire, &LXQt::Panel::Wayfire::viewTitleChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());

        mViews[viewId] = view;

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Title);
    });

    connect(&mWayfire, &LXQt::Panel::Wayfire::viewAppIdChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());

        mViews[viewId] = view;

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::WindowClass);
        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Icon);
    });

    connect(&mWayfire, &LXQt::Panel::Wayfire::viewOutputChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());

        mViews[viewId] = view;
    });

    connect(&mWayfire, &LXQt::Panel::Wayfire::viewWorkspaceChanged, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        /** Filter non-toplevel views */
        if (!isValidToplevel(view))
        {
            return;
        }

        WaylandId viewId(view[QSL("id")].toInt());

        mViews[viewId] = view;

        emit windowPropertyChanged(viewId, (int)LXQtTaskBarWindowProperty::Workspace);
    });

    connect(&mWayfire, &LXQt::Panel::Wayfire::viewUnmapped, [this] ( QJsonDocument respJson )
    {
        QJsonObject response = respJson.object();
        QJsonObject view     = response[QSL("view")].toObject();

        WaylandId viewId(view[QSL("id")].toInt());

        mViews.remove(viewId);

        emit windowRemoved(viewId);
    });

    mWayfire.connectToServer();
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

      /** To be implemented in wayfire ipc */
      case LXQtTaskBarBackendAction::MaximizeVertically:
        return false;

      /** To be implemented in wayfire ipc */
      case LXQtTaskBarBackendAction::MaximizeHorizontally:
        return false;

      case LXQtTaskBarBackendAction::Minimize:
        return true;

      /** Not implemented */
      case LXQtTaskBarBackendAction::RollUp:
        return false;

      case LXQtTaskBarBackendAction::FullScreen:
        return true;

      case LXQtTaskBarBackendAction::DesktopSwitch:
        return true;

      /** Available via wsets plugin */
      case LXQtTaskBarBackendAction::MoveToDesktop:
        return true;

      /** Pin Above and Normal are available */
      case LXQtTaskBarBackendAction::MoveToLayer:
        return true;

      /** Available via wsets plugin */
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
    for (WaylandId viewId : mViews.keys())
    {
        mViews.remove(viewId);
        emit windowRemoved(viewId);
    }

    QJsonArray views = mWayfire.listViews();
    while (views.count())
    {
        QJsonObject view = views.takeAt(0).toObject();
        WaylandId id(view[QSL("id")].toInt());

        qDebug() << id;

        mViews[id] = view;
        emit windowAdded(id);
    }

    qDebug() << "=====================";

    return true;
}

QVector<WId> LXQtTaskbarWayfireBackend::getCurrentWindows() const
{
    QVector<WId> ids;
    for ( WaylandId viewId : mViews.keys())
    {
        ids << viewId;
    }

    qDebug() << "Current windows" << ids;

    return ids;
}

QString LXQtTaskbarWayfireBackend::getWindowTitle(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return QString();
    }

    return mViews[viewId][QSL("title")].toString();
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

    return getIconForAppId(mViews[viewId][QSL("app-id")].toString());
}

QString LXQtTaskbarWayfireBackend::getWindowClass(WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return QString();
    }

    return mViews[viewId][QSL("app-id")].toString();
}

LXQtTaskBarWindowLayer LXQtTaskbarWayfireBackend::getWindowLayer(WId windowId) const
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

    if (mViews[viewId][QSL("mapped")].toBool())
    {
        return LXQtTaskBarWindowState::Hidden;
    }

    if (mViews[viewId][QSL("minimized")].toBool())
    {
        return LXQtTaskBarWindowState::Minimized;
    }

    if (mViews[viewId][QSL("fullscreen")].toBool())
    {
        return LXQtTaskBarWindowState::FullScreen;
    }

    // WLR_EDGE_TOP | WLR_EDGE_BOTTOM | WLR_EDGE_LEFT | WLR_EDGE_RIGHT == 1 | 2 | 4 | 8 == 15
    if (mViews[viewId][QSL("tiled")].toInt() == 15)
    {
        return LXQtTaskBarWindowState::Maximized;
    }

    // WLR_EDGE_TOP | WLR_EDGE_BOTTOM == 1 | 2 == 3
    if (mViews[viewId][QSL("tiled")].toInt() == 3)
    {
        return LXQtTaskBarWindowState::MaximizedVertically;
    }

    // WLR_EDGE_LEFT | WLR_EDGE_RIGHT == 4 | 8 == 12
    if (mViews[viewId][QSL("tiled")].toInt() == 12)
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
        mWayfire.minimizeView(viewId, set);
        break;
    }

      case LXQtTaskBarWindowState::Maximized:
    {
        mWayfire.maximizeView(viewId, (set ? 15 : 0));
        break;
    }

      case LXQtTaskBarWindowState::MaximizedVertically:
    {
        mWayfire.maximizeView(viewId, (set ? 3 : 0));
        break;
    }

      case LXQtTaskBarWindowState::MaximizedHorizontally:
    {
        mWayfire.maximizeView(viewId, (set ? 12 : 0));
        break;
    }

      case LXQtTaskBarWindowState::Normal:
    {
        mWayfire.restoreView(viewId);
        break;
    }

      case LXQtTaskBarWindowState::FullScreen:
    {
        mWayfire.fullscreenView(viewId, set);
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
    return (mWayfire.getActiveView() == viewId);
}

bool LXQtTaskbarWayfireBackend::raiseWindow(WId windowId, bool onCurrentWorkSpace)
{
    Q_UNUSED(onCurrentWorkSpace) // Cannot be done on a generic wlroots-based compositor!

    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return mWayfire.focusView(viewId);
}

bool LXQtTaskbarWayfireBackend::closeWindow(WId windowId)
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return mWayfire.closeView(viewId);
}

WId LXQtTaskbarWayfireBackend::getActiveWindow() const
{
    return mWayfire.getActiveView();
}

int LXQtTaskbarWayfireBackend::getWorkspacesCount() const
{
    QJsonObject wsetsInfo = mWayfire.getWorkspaceSetsInfo().at(0).toObject();
    QJsonObject workspace = wsetsInfo[QSL("workspace")].toObject();
    int64_t nRows = workspace[QSL("grid_height")].toInt();
    int64_t nCols = workspace[QSL("grid_width")].toInt();

    return (nRows * nCols);
}

QString LXQtTaskbarWayfireBackend::getWorkspaceName(int x, QString outputName) const
{
    return mWayfire.getWorkspaceName(x, outputName);
}

int LXQtTaskbarWayfireBackend::getCurrentWorkspace() const
{
    QJsonObject outputInfo = mWayfire.getOutputInfo( mWayfire.getActiveOutput() );
    QJsonObject outputWS   = outputInfo[QSL("workspace")].toObject();

    int nCols  = outputWS[QSL("grid_width")].toInt();  // Total columns in workspace grid
    int curRow = outputWS[QSL("y")].toInt(); // Current workspace row (0-based)
    int curCol = outputWS[QSL("x")].toInt(); // Current workspace column (0-based)

    return curRow * nCols + curCol + 1;
}

bool LXQtTaskbarWayfireBackend::setCurrentWorkspace(int x)
{
    return mWayfire.switchToWorkspace(mWayfire.getActiveOutput(), x);
}

int LXQtTaskbarWayfireBackend::getWindowWorkspace(WId windowId) const
{
    WaylandId viewId(windowId);
    QJsonObject viewInfo = mWayfire.getViewInfo(viewId);
    QJsonObject viewGeom = viewInfo[QSL("geometry")].toObject();

    // Calculate the center of the window
    QPoint viewCenter(
        viewGeom[QSL("x")].toInt() + viewGeom[QSL("width")].toInt() / 2,
        viewGeom[QSL("y")].toInt() + viewGeom[QSL("height")].toInt() / 2
    );

    QJsonObject outputInfo = mWayfire.getOutputInfo(WaylandId(viewInfo[QSL("output-id")].toInt()));
    QJsonObject outputGeom = outputInfo[QSL("geometry")].toObject();
    QJsonObject outputWS   = outputInfo[QSL("workspace")].toObject();

    QRect opGeom(
        outputGeom[QSL("x")].toInt(),
        outputGeom[QSL("y")].toInt(),
        outputGeom[QSL("width")].toInt(),
        outputGeom[QSL("height")].toInt()
    );

    int nRows = outputWS[QSL("grid_height")].toInt(); // Total rows in workspace grid
    int nCols = outputWS[QSL("grid_width")].toInt();  // Total columns in workspace grid

    int currentRow = outputWS[QSL("y")].toInt(); // Current workspace row (0-based)
    int currentCol = outputWS[QSL("x")].toInt(); // Current workspace column (0-based)

    // Calculate the geometries of all workspaces relative to the current workspace
    QHash<int, QRect> wsGeomHash;
    for (int row = 0; row < nRows; ++row)
    {
        for (int col = 0; col < nCols; ++col)
        {
            // Workspace index (0-based)
            int wsIndex = row * nCols + col;

            // Workspace geometry (relative to the current workspace)
            QRect wsGeom(
                opGeom.x() + (col - currentCol) * opGeom.width(),
                opGeom.y() + (row - currentRow) * opGeom.height(),
                opGeom.width(),
                opGeom.height()
            );

            wsGeomHash[wsIndex] = wsGeom;
        }
    }

    // Find which workspace contains the view's center
    for (auto it = wsGeomHash.constBegin(); it != wsGeomHash.constEnd(); ++it)
    {
        if (it.value().contains(viewCenter))
        {
            return it.key();
        }
    }

    // Fallback: If not found, assume current workspace
    return currentRow * nCols + currentCol;
}

bool LXQtTaskbarWayfireBackend::setWindowOnWorkspace(WId windowId, int idx)
{
    WaylandId viewId(windowId);
    return mWayfire.sendViewToWorkspace(viewId, idx);
}

void LXQtTaskbarWayfireBackend::moveApplicationToPrevNextMonitor(WId viewId, bool nextOp, bool raiseWindow)
{
    // 1. Get the current output id and its active wset-id
    // Get view info to find which output it's currently on
    QJsonObject viewInfo = mWayfire.getViewInfo(WaylandId(viewId));
    if (viewInfo.isEmpty())
    {
        qWarning() << "Failed to get view info for view" << viewId;
        return;
    }

    QJsonObject currentOutputInfo = viewInfo[QSL("output")].toObject();
    if (currentOutputInfo.isEmpty())
    {
        qWarning() << "View" << viewId << "is not on any output";
        return;
    }

    WaylandId currentOutputId(currentOutputInfo[QSL("id")].toInt());

    // Get all outputs
    QJsonArray outputs = mWayfire.listOutputs();
    if (outputs.isEmpty())
    {
        qWarning() << "No outputs available";
        return;
    }

    // 2. Find the previous/next output
    int currentIndex = -1;
    for (int i = 0; i < outputs.size(); i++)
    {
        QJsonObject output = outputs[i].toObject();
        if (output[QSL("id")].toInt() == currentOutputId.id)
        {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex == -1)
    {
        qWarning() << "Current output not found in outputs list";
        return;
    }

    // Calculate target output index with wrap-around
    int targetIndex;
    if (nextOp)
    {
        targetIndex = (currentIndex + 1) % outputs.size();
    } else
    {
        targetIndex = (currentIndex - 1 + outputs.size()) % outputs.size();
    }

    QJsonObject targetOutput = outputs[targetIndex].toObject();
    WaylandId targetOutputId(targetOutput[QSL("id")].toInt());

    // 3. Move the view to target output's workspace set
    // Get workspace sets info
    QJsonArray wsets = mWayfire.getWorkspaceSetsInfo();
    if (wsets.isEmpty())
    {
        qWarning() << "No workspace sets available";
        return;
    }

    // Find the target output's active workspace set
    WaylandId targetWsetId(0);
    for (const QJsonValue & wsVal : wsets)
    {
        QJsonObject ws = wsVal.toObject();
        if (ws[QSL("output-id")].toInt() == targetOutputId.id)
        {
            targetWsetId = WaylandId(ws[QSL("id")].toInt());
            break;
        }
    }

    if (targetWsetId.id == 0)
    {
        qWarning() << "Failed to find workspace set for target output";
        return;
    }

    // Move the view to target workspace set
    QJsonObject moveRequest;
    moveRequest[QSL("method")] = QSL("window-rules/move-view-to-wset");
    moveRequest[QSL("data")]   = QJsonObject{
        {QSL("id"), QJsonValue::fromVariant((quint64)viewId)},
        {QSL("wset-id"), QJsonValue::fromVariant((quint64)targetWsetId.id)}
    };

    QJsonDocument reply = mWayfire.genericRequest(QJsonDocument(moveRequest));
    if (reply[QSL("result")].toString() != QSL("ok"))
    {
        qWarning() << "Failed to move view to target workspace set:" << reply.toJson();
        return;
    }

    // 4. Focus the window if requested
    if (raiseWindow)
    {
        mWayfire.focusView(WaylandId(viewId));
    }
}

bool LXQtTaskbarWayfireBackend::isWindowOnScreen(QScreen *scrn, WId windowId) const
{
    WaylandId viewId(windowId);
    if (!mViews.contains(viewId))
    {
        return false;
    }

    return mViews[viewId][QSL("output-name")] == scrn->name();
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

    return mWayfire.showDesktop(mWayfire.getActiveOutput());
}

int LXQtWMBackendWayfireLibrary::getBackendScore(const QString& key) const
{
    // Only wayfire is supported
    if ((key == QSL("wayfire")) || (key == QSL("Wayfire")))
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
