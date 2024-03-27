#pragma once

#include <QIcon>
#include <QPointer>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "wayland-wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

typedef quintptr WId;

class LXQtTaskBarWlrootsWindow;

class LXQtTaskBarWlrootsWindowManagment : public QWaylandClientExtensionTemplate<LXQtTaskBarWlrootsWindowManagment>,
                                         public QtWayland::zwlr_foreign_toplevel_manager_v1
{
    Q_OBJECT
public:
    static constexpr int version = 16;

    LXQtTaskBarWlrootsWindowManagment();
    ~LXQtTaskBarWlrootsWindowManagment();

    inline bool isShowingDesktop() const { return m_isShowingDesktop; }

protected:
    void zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel);
    void zwlr_foreign_toplevel_manager_v1_finished() {};

Q_SIGNALS:
    void windowCreated(LXQtTaskBarWlrootsWindow *window);

private:
    bool m_isShowingDesktop = false;
};

class LXQtTaskBarWlrootsWindow : public QObject,
                                public QtWayland::zwlr_foreign_toplevel_handle_v1
{
    Q_OBJECT
public:
    LXQtTaskBarWlrootsWindow(::zwlr_foreign_toplevel_handle_v1 *id);
    ~LXQtTaskBarWlrootsWindow();

    inline WId getWindowId() const { return reinterpret_cast<WId>(this); }

    void activate();

    using state = QtWayland::zwlr_foreign_toplevel_handle_v1::state;
    QString title;
    QString appId;
    QIcon icon;
    QFlags<state> windowState;
    QPointer<LXQtTaskBarWlrootsWindow> parentWindow;

    void set_state( LXQtTaskBarWlrootsWindow::state, bool ) {};

Q_SIGNALS:
    void titleChanged();
    void appIdChanged();
    void outputEnter();
    void outputLeave();
    void activeChanged();
    void maximizedChanged();
    void minimizedChanged();
    void fullscreenChanged();
    void done();
    void closed();
    void parentChanged();

    /** We wait to get the title and appId to emit this */
    void windowReady();

protected:
    void zwlr_foreign_toplevel_handle_v1_title(const QString &title);
    void zwlr_foreign_toplevel_handle_v1_app_id(const QString &app_id);
    void zwlr_foreign_toplevel_handle_v1_output_enter(struct ::wl_output *output);
    void zwlr_foreign_toplevel_handle_v1_output_leave(struct ::wl_output *output);
    void zwlr_foreign_toplevel_handle_v1_state(wl_array *state);
    void zwlr_foreign_toplevel_handle_v1_done();
    void zwlr_foreign_toplevel_handle_v1_closed();
    void zwlr_foreign_toplevel_handle_v1_parent(struct ::zwlr_foreign_toplevel_handle_v1 *parent);

private:
    void setParentWindow(LXQtTaskBarWlrootsWindow *parent);

    QMetaObject::Connection parentWindowUnmappedConnection;

    mutable bool titleRecieved = false;
    mutable bool appIdRecieved = false;
};
