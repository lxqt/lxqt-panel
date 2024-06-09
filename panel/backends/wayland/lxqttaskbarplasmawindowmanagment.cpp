/*
    SPDX-FileCopyrightText: 2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

    Adapted from KDE Plasma Workspace: plasma-workspace/libtaskmanager/waylandtasksmodel.cpp
*/

#include "lxqttaskbarplasmawindowmanagment.h"

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
 * LXQtTaskBarPlasmaWindow
 */

LXQtTaskBarPlasmaWindow::LXQtTaskBarPlasmaWindow(const QString &uuid, ::org_kde_plasma_window *id)
    : org_kde_plasma_window(id)
    , uuid(uuid)
{
}

LXQtTaskBarPlasmaWindow::~LXQtTaskBarPlasmaWindow()
{
    destroy();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_unmapped()
{
    wasUnmapped = true;
    Q_EMIT unmapped();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_title_changed(const QString &title)
{
    if(this->title == title)
        return;
    this->title = title;
    Q_EMIT titleChanged();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_app_id_changed(const QString &app_id)
{
    if(appId == app_id)
        return;
    appId = app_id;
    Q_EMIT appIdChanged();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_icon_changed()
{
    int pipeFds[2];
    if (pipe2(pipeFds, O_CLOEXEC) != 0) {
        qWarning() << "TaskManager: failed creating pipe";
        return;
    }
    get_icon(pipeFds[1]);
    ::close(pipeFds[1]);
    auto readIcon = [uuid = uuid](int fd) {
        auto closeGuard = qScopeGuard([fd]() {
            ::close(fd);
        });
        pollfd pollFd;
        pollFd.fd = fd;
        pollFd.events = POLLIN;
        QByteArray data;
        while (true) {
            int ready = poll(&pollFd, 1, 1000);
            if (ready < 0 && errno != EINTR) {
                qWarning() << "TaskManager: polling for icon of window" << uuid << "failed";
                return QIcon();
            } else if (ready == 0) {
                qWarning() << "TaskManager: time out polling for icon of window" << uuid;
                return QIcon();
            } else {
                char buffer[4096];
                int n = read(fd, buffer, sizeof(buffer));
                if (n < 0) {
                    qWarning() << "TaskManager: error reading icon of window" << uuid;
                    return QIcon();
                } else if (n > 0) {
                    data.append(buffer, n);
                } else {
                    QIcon icon;
                    QDataStream ds(data);
                    ds >> icon;
                    return icon;
                }
            }
        }
    };
    QFuture<QIcon> future = QtConcurrent::run(readIcon, pipeFds[0]);
    auto watcher = new QFutureWatcher<QIcon>();
    watcher->setFuture(future);
    connect(watcher, &QFutureWatcher<QIcon>::finished, this, [this, watcher] {
        icon = watcher->future().result();
        Q_EMIT iconChanged();
    });
    connect(watcher, &QFutureWatcher<QIcon>::finished, watcher, &QObject::deleteLater);
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_themed_icon_name_changed(const QString &name)
{
    icon = QIcon::fromTheme(name);
    Q_EMIT iconChanged();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_state_changed(uint32_t flags)
{
    auto diff = windowState ^ flags;
    if (diff & state::state_active) {
        windowState.setFlag(state::state_active, flags & state::state_active);
        Q_EMIT activeChanged();
    }
    if (diff & state::state_minimized) {
        windowState.setFlag(state::state_minimized, flags & state::state_minimized);
        Q_EMIT minimizedChanged();
    }
    if (diff & state::state_maximized) {
        windowState.setFlag(state::state_maximized, flags & state::state_maximized);
        Q_EMIT maximizedChanged();
    }
    if (diff & state::state_fullscreen) {
        windowState.setFlag(state::state_fullscreen, flags & state::state_fullscreen);
        Q_EMIT fullscreenChanged();
    }
    if (diff & state::state_keep_above) {
        windowState.setFlag(state::state_keep_above, flags & state::state_keep_above);
        Q_EMIT keepAboveChanged();
    }
    if (diff & state::state_keep_below) {
        windowState.setFlag(state::state_keep_below, flags & state::state_keep_below);
        Q_EMIT keepBelowChanged();
    }
    if (diff & state::state_on_all_desktops) {
        windowState.setFlag(state::state_on_all_desktops, flags & state::state_on_all_desktops);
        Q_EMIT onAllDesktopsChanged();
    }
    if (diff & state::state_demands_attention) {
        windowState.setFlag(state::state_demands_attention, flags & state::state_demands_attention);
        Q_EMIT demandsAttentionChanged();
    }
    if (diff & state::state_closeable) {
        windowState.setFlag(state::state_closeable, flags & state::state_closeable);
        Q_EMIT closeableChanged();
    }
    if (diff & state::state_minimizable) {
        windowState.setFlag(state::state_minimizable, flags & state::state_minimizable);
        Q_EMIT minimizeableChanged();
    }
    if (diff & state::state_maximizable) {
        windowState.setFlag(state::state_maximizable, flags & state::state_maximizable);
        Q_EMIT maximizeableChanged();
    }
    if (diff & state::state_fullscreenable) {
        windowState.setFlag(state::state_fullscreenable, flags & state::state_fullscreenable);
        Q_EMIT fullscreenableChanged();
    }
    if (diff & state::state_skiptaskbar) {
        windowState.setFlag(state::state_skiptaskbar, flags & state::state_skiptaskbar);
        Q_EMIT skipTaskbarChanged();
    }
    if (diff & state::state_shadeable) {
        windowState.setFlag(state::state_shadeable, flags & state::state_shadeable);
        Q_EMIT shadeableChanged();
    }
    if (diff & state::state_shaded) {
        windowState.setFlag(state::state_shaded, flags & state::state_shaded);
        Q_EMIT shadedChanged();
    }
    if (diff & state::state_movable) {
        windowState.setFlag(state::state_movable, flags & state::state_movable);
        Q_EMIT movableChanged();
    }
    if (diff & state::state_resizable) {
        windowState.setFlag(state::state_resizable, flags & state::state_resizable);
        Q_EMIT resizableChanged();
    }
    if (diff & state::state_virtual_desktop_changeable) {
        windowState.setFlag(state::state_virtual_desktop_changeable, flags & state::state_virtual_desktop_changeable);
        Q_EMIT virtualDesktopChangeableChanged();
    }
    if (diff & state::state_skipswitcher) {
        windowState.setFlag(state::state_skipswitcher, flags & state::state_skipswitcher);
        Q_EMIT skipSwitcherChanged();
    }
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_virtual_desktop_entered(const QString &id)
{
    virtualDesktops.push_back(id);
    Q_EMIT virtualDesktopEntered();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_virtual_desktop_left(const QString &id)
{
    virtualDesktops.removeAll(id);
    Q_EMIT virtualDesktopLeft();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_geometry(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    geometry = QRect(x, y, width, height);
    Q_EMIT geometryChanged();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_application_menu(const QString &service_name, const QString &object_path)
{
    applicationMenuService = service_name;
    applicationMenuObjectPath = object_path;
    Q_EMIT applicationMenuChanged();
}

void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_activity_entered(const QString &id)
{
    activities.push_back(id);
    Q_EMIT activitiesChanged();
}
void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_activity_left(const QString &id)
{
    activities.removeAll(id);
    Q_EMIT activitiesChanged();
}
void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_pid_changed(uint32_t pid)
{
    this->pid = pid;
}
void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_resource_name_changed(const QString &resource_name)
{
    resourceName = resource_name;
}
void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_parent_window(::org_kde_plasma_window *parent)
{
    LXQtTaskBarPlasmaWindow *parentWindow = nullptr;
    if (parent) {
        parentWindow = dynamic_cast<LXQtTaskBarPlasmaWindow *>(LXQtTaskBarPlasmaWindow::fromObject(parent));
    }
    setParentWindow(parentWindow);
}
void LXQtTaskBarPlasmaWindow::org_kde_plasma_window_initial_state()
{
    Q_EMIT initialStateDone();
}

void LXQtTaskBarPlasmaWindow::setParentWindow(LXQtTaskBarPlasmaWindow *parent)
{
    const auto old = parentWindow;
    QObject::disconnect(parentWindowUnmappedConnection);

    if (parent && !parent->wasUnmapped) {
        parentWindow = QPointer<LXQtTaskBarPlasmaWindow>(parent);
        parentWindowUnmappedConnection = QObject::connect(parent, &LXQtTaskBarPlasmaWindow::unmapped, this, [this] {
            setParentWindow(nullptr);
        });
    } else {
        parentWindow = QPointer<LXQtTaskBarPlasmaWindow>();
        parentWindowUnmappedConnection = QMetaObject::Connection();
    }

    if (parentWindow.data() != old.data()) {
        Q_EMIT parentWindowChanged();
    }
}

/*
 * LXQtTaskBarPlasmaWindowManagment
 */

LXQtTaskBarPlasmaWindowManagment::LXQtTaskBarPlasmaWindowManagment()
    : QWaylandClientExtensionTemplate(version)
{
    connect(this, &QWaylandClientExtension::activeChanged, this, [this] {
        if (!isActive()) {
            org_kde_plasma_window_management_destroy(object());
        }
    });
}

LXQtTaskBarPlasmaWindowManagment::~LXQtTaskBarPlasmaWindowManagment()
{
    if (isActive()) {
        org_kde_plasma_window_management_destroy(object());
    }
}

void LXQtTaskBarPlasmaWindowManagment::org_kde_plasma_window_management_show_desktop_changed(uint32_t state)
{
    m_isShowingDesktop = (state == show_desktop::show_desktop_enabled);
}

void LXQtTaskBarPlasmaWindowManagment::org_kde_plasma_window_management_window_with_uuid(uint32_t id, const QString &uuid)
{
    Q_UNUSED(id)
    Q_EMIT windowCreated(new LXQtTaskBarPlasmaWindow(uuid, get_window_by_uuid(uuid)));
}
void LXQtTaskBarPlasmaWindowManagment::org_kde_plasma_window_management_stacking_order_uuid_changed(const QString &uuids)
{
    Q_EMIT stackingOrderChanged(uuids);
}
