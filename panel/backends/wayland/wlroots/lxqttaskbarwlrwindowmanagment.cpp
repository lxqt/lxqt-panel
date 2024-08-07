#include "lxqttaskbarwlrwindowmanagment.h"

#include <QString>
#include <QFuture>
#include <QtConcurrent>
#include <QGuiApplication>
#include <QMimeData>
#include <QSet>
#include <QUrl>
#include <QUuid>
#include <QWaylandClientExtension>
#include <QWindow>

#include <xdgicon.h>

#include <qpa/qplatformnativeinterface.h>

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

#include <filesystem>

QString U8Str( const char *str ) {
    return QString::fromUtf8( str );
}

static inline QString getPixmapIcon(QString name)
{
    QStringList paths{
        U8Str("/usr/local/share/pixmaps/"),
        U8Str("/usr/share/pixmaps/"),
    };

    QStringList sfxs{
        U8Str( ".svg" ), U8Str( ".png" ), U8Str( ".xpm" )
    };

    for (QString path: paths)
    {
        for (QString sfx: sfxs)
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
     * Assume mAppId == desktop-file-name (ideal situation)
     * or mAppId.toLower() == desktop-file-name (cheap fallback)
     */
    QString iconName;

    for (QString path: appDirs)
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
    for (QString path: appDirs)
    {
        QStringList desktops = QDir(path).entryList({ U8Str("*.desktop") });
        for (QString dskf: desktops)
        {
            QSettings desktop(path + dskf, QSettings::IniFormat);

            QString exec = desktop.value(U8Str("Desktop Entry/Exec"), U8Str("abcd1234/-")).toString();
            QString name = desktop.value(U8Str("Desktop Entry/Name"), U8Str("abcd1234/-")).toString();
            QString cls  = desktop.value(U8Str("Desktop Entry/StartupWMClass"), U8Str("abcd1234/-")).toString();

            QString execPath = U8Str(std::filesystem::path(exec.toStdString()).filename().c_str());

            if (mAppId.compare(execPath, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            }

            else if (mAppId.compare(name, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            }

            else if (mAppId.compare(cls, Qt::CaseInsensitive) == 0)
            {
                iconName = desktop.value(U8Str("Desktop Entry/Icon")).toString();
            }

            if (not iconName.isEmpty())
            {
                if (QIcon::hasThemeIcon(iconName))
                {
                    return QIcon::fromTheme(iconName);
                }

                else if (QFile::exists(iconName))
                {
                    return QIcon(iconName);
                }

                else
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


static inline wl_seat *get_seat()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if (!native)
    {
        return nullptr;
    }

    struct wl_seat *seat = reinterpret_cast<wl_seat *>(native->nativeResourceForIntegration("wl_seat"));

    return seat;
}


/*
 * LXQtTaskbarWlrootsWindowManagment
 */

LXQtTaskbarWlrootsWindowManagment::LXQtTaskbarWlrootsWindowManagment() : QWaylandClientExtensionTemplate(version)
{
    /** Automatically destroy thie object */
    connect(
        this, &QWaylandClientExtension::activeChanged, this, [ this ] {
        if (!isActive())
        {
            zwlr_foreign_toplevel_manager_v1_destroy(object());
        }
    });
}


LXQtTaskbarWlrootsWindowManagment::~LXQtTaskbarWlrootsWindowManagment()
{
    if (isActive())
    {
        zwlr_foreign_toplevel_manager_v1_destroy(object());
    }
}


void LXQtTaskbarWlrootsWindowManagment::zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel)
{
    /**
     * A window was created.
     * Wait for the window to become ready, i.e. wait for done() event to be sent by the compositor.
     * Once we recieve done(), emit the windowReady() signal.
     */

    auto w = new LXQtTaskbarWlrootsWindow(toplevel);

    connect(w, &LXQtTaskbarWlrootsWindow::windowReady, [w, this] () {
        emit windowCreated(w->getWindowId());
    });
}


/*
 * LXQtTaskbarWlrootsWindow
 */

LXQtTaskbarWlrootsWindow::LXQtTaskbarWlrootsWindow(::zwlr_foreign_toplevel_handle_v1 *id) : zwlr_foreign_toplevel_handle_v1(id)
{
}


LXQtTaskbarWlrootsWindow::~LXQtTaskbarWlrootsWindow()
{
    destroy();
}


void LXQtTaskbarWlrootsWindow::activate()
{
    /**
     * Activate on default seat.
     * TODO: Worry about multi-seat setups, when we have no other worries :P
     */
    zwlr_foreign_toplevel_handle_v1::activate(get_seat());
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_title(const QString& title)
{
    /** Store the incoming title in pending */
    m_pendingState.title        = title;
    m_pendingState.titleChanged = true;
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_app_id(const QString& app_id)
{
    /** Store the incoming appId in pending */
    m_pendingState.appId        = app_id;
    m_pendingState.appIdChanged = true;

    /** Update the icon */
    this->icon = getIconForAppId(app_id);

    /** We did not get any icon from app-id. Let's use application-x-executable */
    if (this->icon.pixmap(64).width() == 0)
    {
        this->icon = XdgIcon::fromTheme(QString::fromUtf8("application-x-executable"));
    }
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_output_enter(struct ::wl_output *output)
{
    /** This view was added to an output */
    m_pendingState.outputs << output;
    m_pendingState.outputsChanged = true;
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_output_leave(struct ::wl_output *output)
{
    /** This view was removed from an output; store it in pending. */
    m_pendingState.outputsLeft << output;

    if (m_pendingState.outputs.contains(output))
    {
        m_pendingState.outputs.removeAll(output);
    }

    m_pendingState.outputsChanged = true;
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_state(wl_array *state)
{
    /** State of this window was changed; store it in pending. */
    auto *states    = static_cast<uint32_t *>(state->data);
    int   numStates = static_cast<int>(state->size / sizeof(uint32_t));

    for (int i = 0; i < numStates; i++)
    {
        switch ((uint32_t)states[ i ])
        {
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED: {
            m_pendingState.maximized        = true;
            m_pendingState.maximizedChanged = true;
            break;
        }

        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED: {
            m_pendingState.minimized        = true;
            m_pendingState.minimizedChanged = true;
            break;
        }

        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED: {
            m_pendingState.activated        = true;
            m_pendingState.activatedChanged = true;
            break;
        }

        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN: {
            m_pendingState.fullscreen        = true;
            m_pendingState.fullscreenChanged = true;
            break;
        }
        }
    }
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_done()
{
    /**
     * All the states/properties have been sent.
     * We can now emit the signals and clear the pending state:
     * 1. Update all the variables first.
     * 2. Then clear the m_pendingState.<variable>
     * 3. Emit the changed signals.
     * 4. Finally, cleanr the m_pendingState.<variable>Changed flags.
     */

    // (1) title, if it changed
    if (m_pendingState.titleChanged)
    {
        windowState.title = m_pendingState.title;
    }

    // (2) appId, if it changed
    if (m_pendingState.appIdChanged)
    {
        windowState.appId = m_pendingState.appId;
    }

    // (3) outputs, if they changed
    if (m_pendingState.outputsChanged)
    {
        for (::wl_output *op: m_pendingState.outputsLeft)
        {
            if (windowState.outputs.contains(op))
            {
                windowState.outputs.removeAll(op);
            }
        }

        for (::wl_output *op: m_pendingState.outputs)
        {
            if (!windowState.outputs.contains(op))
            {
                windowState.outputs << op;
            }
        }
    }

    // (4) states, if they changed. Don't trust the changed flag.
    if (m_pendingState.maximized != windowState.maximized)
    {
        windowState.maximized           = m_pendingState.maximized;
        m_pendingState.maximizedChanged = true;
    }

    if (m_pendingState.minimized != windowState.minimized)
    {
        windowState.minimized           = m_pendingState.minimized;
        m_pendingState.minimizedChanged = true;
    }

    if (m_pendingState.activated != windowState.activated)
    {
        windowState.activated           = m_pendingState.activated;
        m_pendingState.activatedChanged = true;
    }

    if (m_pendingState.fullscreen != windowState.fullscreen)
    {
        windowState.fullscreen           = m_pendingState.fullscreen;
        m_pendingState.fullscreenChanged = true;
    }

    // (5) parent, if it changed.
    if (m_pendingState.parentChanged)
    {
        if (m_pendingState.parent)
        {
            setParentWindow(new LXQtTaskbarWlrootsWindow(m_pendingState.parent));
        }

        else
        {
            setParentWindow(nullptr);
        }
    }

    /** 2. Clear all m_pendingState.<variables> for next run */
    m_pendingState.title = QString();
    m_pendingState.appId = QString();
    m_pendingState.outputs.clear();
    m_pendingState.maximized  = false;
    m_pendingState.minimized  = false;
    m_pendingState.activated  = false;
    m_pendingState.fullscreen = false;
    m_pendingState.parent     = nullptr;

    /**
     * 3. Emit signals
     *    (a) First time done was emitted after the window was created.
     *    (b) Other times.
     */

    /** (a) First time done was emitted */
    if (initDone == false)
    {
        /**
         * All the states/properties are already set.
         * Any query will give valid results.
         */
        initDone = true;
        emit windowReady();
    }

    /** (b) All the subsequent times */
    else
    {
        if (m_pendingState.titleChanged)
            emit titleChanged();
        if (m_pendingState.appIdChanged)
            emit appIdChanged();
        if (m_pendingState.outputsChanged)
            emit outputsChanged();
        if (m_pendingState.maximizedChanged)
            emit maximizedChanged();
        if (m_pendingState.minimizedChanged)
            emit minimizedChanged();
        if (m_pendingState.activatedChanged)
            emit activatedChanged();
        if (m_pendingState.fullscreenChanged)
            emit fullscreenChanged();
        if (m_pendingState.parentChanged)
            emit parentChanged();

        emit stateChanged();
    }

    /** 4. Clear m+m_pendingState.<variable>Changed flags */
    m_pendingState.titleChanged      = false;
    m_pendingState.appIdChanged      = false;
    m_pendingState.outputsChanged    = false;
    m_pendingState.maximizedChanged  = false;
    m_pendingState.minimizedChanged  = false;
    m_pendingState.activatedChanged  = false;
    m_pendingState.fullscreenChanged = false;
    m_pendingState.parentChanged     = false;
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_closed()
{
    /** This window was closed */
    emit closed();
}


void LXQtTaskbarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent)
{
    /** Parent of this window changed; store it in pending. */
    m_pendingState.parent        = parent;
    m_pendingState.parentChanged = true;
}


void LXQtTaskbarWlrootsWindow::setParentWindow(LXQtTaskbarWlrootsWindow *parent)
{
    QObject::disconnect(parentWindowUnmappedConnection);

    if (parent)
    {
        parentWindow = parent->getWindowId();
        parentWindowUnmappedConnection = QObject::connect(
            parent, &LXQtTaskbarWlrootsWindow::closed, this, [ this ] {
            setParentWindow(nullptr);
        });
    }
    else
    {
        parentWindow = 0;
        parentWindowUnmappedConnection = QMetaObject::Connection();
    }
}
