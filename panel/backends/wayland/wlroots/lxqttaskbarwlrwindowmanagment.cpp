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

wl_seat *get_seat() {
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    if ( !native ) {
        return nullptr;
    }

    struct wl_seat *seat = reinterpret_cast<wl_seat *>(native->nativeResourceForIntegration( "wl_seat" ) );

    return seat;
}

/*
 * LXQtTaskBarWlrootsWindowManagment
 */

LXQtTaskBarWlrootsWindowManagment::LXQtTaskBarWlrootsWindowManagment()
    : QWaylandClientExtensionTemplate(version)
{
    connect(this, &QWaylandClientExtension::activeChanged, this, [this] {
        if (!isActive()) {
            zwlr_foreign_toplevel_manager_v1_destroy(object());
        }
    });
}

LXQtTaskBarWlrootsWindowManagment::~LXQtTaskBarWlrootsWindowManagment()
{
    if (isActive()) {
        zwlr_foreign_toplevel_manager_v1_destroy(object());
    }
}

void LXQtTaskBarWlrootsWindowManagment::zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel)
{
    emit windowCreated( new LXQtTaskBarWlrootsWindow(toplevel) );
}


/*
 * LXQtTaskBarWlrootsWindow
 */

LXQtTaskBarWlrootsWindow::LXQtTaskBarWlrootsWindow(::zwlr_foreign_toplevel_handle_v1 *id)
    : zwlr_foreign_toplevel_handle_v1(id)
{
    title = QString::fromUtf8( "untitled" );
    appId = QString::fromUtf8( "unknown" );
}

LXQtTaskBarWlrootsWindow::~LXQtTaskBarWlrootsWindow()
{
    destroy();
}

void LXQtTaskBarWlrootsWindow::activate()
{
    zwlr_foreign_toplevel_handle_v1::activate(get_seat());
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_title(const QString &title)
{
    this->title = title;
    titleRecieved = true;
    emit titleChanged();

    if ( titleRecieved && appIdRecieved )
    {
        emit windowReady();
    }
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_app_id(const QString &app_id)
{
    this->appId = app_id;
    appIdRecieved = true;
    emit appIdChanged();

    this->icon = XdgIcon::fromTheme(appId);
    if ( this->icon.pixmap(64).width() == 0 )
    {
        this->icon = XdgIcon::fromTheme(appId.toLower());
    }

    if ( appIdRecieved && titleRecieved )
    {
        emit windowReady();
    }
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_output_enter(struct ::wl_output *output)
{
    emit outputEnter();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_output_leave(struct ::wl_output *output)
{
    emit outputLeave();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_state(wl_array *state)
{
    auto *states   = static_cast<uint32_t *>(state->data);
    int  numStates = static_cast<int>(state->size / sizeof(uint32_t) );

    for ( int i = 0; i < numStates; i++ ) {
        switch ( (uint32_t)states[ i ] ) {
            case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED: {
                m_pendingState.maximized = true;
                break;
            }

            case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED: {
                m_pendingState.minimized = true;
                break;
            }

            case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED: {
                m_pendingState.activated = true;
                break;
            }

            case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN: {
                m_pendingState.fullscreen = true;
                break;
            }
        }
    }
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_done()
{
    /**
     * Update windowState flags before emitting the signals.
     * Otherwise, windowState.testFlag(...) will return wrong information!!
     */
    windowState = QFlags<state>();
    if ( m_pendingState.maximized ) windowState |= state_maximized;
    if ( m_pendingState.minimized ) windowState |= state_minimized;
    if ( m_pendingState.activated ) windowState |= state_activated;
    if ( m_pendingState.fullscreen ) windowState |= state_fullscreen;

    /** Emit the signals. */
    if ( m_viewState.maximized != m_pendingState.maximized )
        emit maximizedChanged();

    if ( m_viewState.minimized != m_pendingState.minimized )
        emit minimizedChanged();

    if ( m_viewState.activated!= m_pendingState.activated )
        emit activeChanged();

    if ( m_viewState.fullscreen != m_pendingState.fullscreen )
        emit fullscreenChanged();

    /** Store m_pendingState into m_viewState for the next run */
    m_viewState.maximized = m_pendingState.maximized;
    m_viewState.minimized = m_pendingState.minimized;
    m_viewState.activated = m_pendingState.activated;
    m_viewState.fullscreen = m_pendingState.fullscreen;

    /** Reset m_pendingState for the next run */
    m_pendingState.maximized = false;
    m_pendingState.minimized = false;
    m_pendingState.activated = false;
    m_pendingState.fullscreen = false;
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_closed()
{
    emit closed();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent)
{
    // setParentWindow(new LXQtTaskBarWlrootsWindow(parent));
}

void LXQtTaskBarWlrootsWindow::setParentWindow(LXQtTaskBarWlrootsWindow *parent)
{
    const auto old = parentWindow;
    QObject::disconnect(parentWindowUnmappedConnection);

    if (parent) {
        parentWindow = QPointer<LXQtTaskBarWlrootsWindow>(parent);
        parentWindowUnmappedConnection = QObject::connect(parent, &LXQtTaskBarWlrootsWindow::closed, this, [this] {
            setParentWindow(nullptr);
        });
    } else {
        parentWindow = QPointer<LXQtTaskBarWlrootsWindow>();
        parentWindowUnmappedConnection = QMetaObject::Connection();
    }

    if (parentWindow.data() != old.data()) {
        Q_EMIT parentChanged();
    }
}
