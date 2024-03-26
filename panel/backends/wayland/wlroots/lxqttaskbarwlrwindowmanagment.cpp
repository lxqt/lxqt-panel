#include "lxqttaskbarwlrwindowmanagment.h"

#include <QFuture>
#include <QtConcurrent>
#include <QGuiApplication>
#include <QMimeData>
#include <QSet>
#include <QUrl>
#include <QUuid>
#include <QWaylandClientExtension>
#include <QWindow>

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

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
    qDebug() << "new toplevel created";
    emit windowCreated( new LXQtTaskBarWlrootsWindow(toplevel) );
}


/*
 * LXQtTaskBarWlrootsWindow
 */

LXQtTaskBarWlrootsWindow::LXQtTaskBarWlrootsWindow(::zwlr_foreign_toplevel_handle_v1 *id)
    : zwlr_foreign_toplevel_handle_v1(id)
{
}

LXQtTaskBarWlrootsWindow::~LXQtTaskBarWlrootsWindow()
{
    destroy();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_title(const QString &title)
{
    emit titleChanged();
    this->title = title;
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_app_id(const QString &app_id)
{
    emit appIdChanged();
    this->appId = app_id;
    // Code to get the icon needs to be inserted here
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
    QFlags<LXQtTaskBarWlrootsWindow::state> wlrState;

    uint32_t* statePtr = static_cast<uint32_t*>(state->data);
    for (size_t i = 0; i < state->size / sizeof(uint32_t); i++) {
        switch (statePtr[i]) {
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MAXIMIZED:
            wlrState |= state_maximized;
            break;
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_FULLSCREEN:
            wlrState |= state_fullscreen;
            break;
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_MINIMIZED:
            wlrState |= state_minimized;
            break;
        case ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED:
            wlrState |= state_activated;
            break;
        }
    }

    if ( windowState.testFlag( state_maximized ) ^ wlrState.testFlag( state_maximized ) )
        emit maximizedChanged();
    if ( windowState.testFlag( state_minimized ) ^ wlrState.testFlag( state_minimized ) )
        emit minimizedChanged();
    if ( windowState.testFlag( state_fullscreen ) ^ wlrState.testFlag( state_fullscreen ) )
        emit fullscreenChanged();
    if ( windowState.testFlag( state_activated ) ^ wlrState.testFlag( state_activated ) )
        emit activeChanged();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_done()
{
    emit done();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_closed()
{
    emit closed();
}

void LXQtTaskBarWlrootsWindow::zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent)
{
    setParentWindow( new LXQtTaskBarWlrootsWindow(parent));
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
