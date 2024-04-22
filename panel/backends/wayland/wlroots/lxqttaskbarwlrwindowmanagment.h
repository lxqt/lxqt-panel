#pragma once

#include <QIcon>
#include <QPointer>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include "qwayland-wlr-foreign-toplevel-management-unstable-v1.h"
#include "wayland-wlr-foreign-toplevel-management-unstable-v1-client-protocol.h"

typedef quintptr WId;

class LXQtTaskbarWlrootsWindow;

class LXQtTaskbarWlrootsWindowManagment : public QWaylandClientExtensionTemplate<LXQtTaskbarWlrootsWindowManagment>,
                                         public QtWayland::zwlr_foreign_toplevel_manager_v1
{
    Q_OBJECT
public:
    static constexpr int version = 16;

    LXQtTaskbarWlrootsWindowManagment();
    ~LXQtTaskbarWlrootsWindowManagment();

    inline bool isShowingDesktop() const { return m_isShowingDesktop; }

protected:
    void zwlr_foreign_toplevel_manager_v1_toplevel(struct ::zwlr_foreign_toplevel_handle_v1 *toplevel);
    void zwlr_foreign_toplevel_manager_v1_finished() {};

Q_SIGNALS:
    void windowCreated(WId wid);

private:
    bool m_isShowingDesktop = false;
};

using WindowState = QtWayland::zwlr_foreign_toplevel_handle_v1::state;

class WindowProperties {
    public:
        /** Title of the window */
        QString title = QString::fromUtf8( "untitled" );
        bool titleChanged = false;

        /** appId of the window */
        QString appId = QString::fromUtf8( "unidentified" );
        bool appIdChanged = false;

        /** List of outputs which the window is currently on */
        QList<::wl_output *> outputs;
        bool outputsChanged = false;

        /** Is maximized */
        bool maximized = false;
        bool maximizedChanged = false;

        /** Is minimized */
        bool minimized = false;
        bool minimizedChanged = false;

        /** Is active */
        bool activated = false;
        bool activatedChanged = false;

        /** Is fullscreen */
        bool fullscreen = false;
        bool fullscreenChanged = false;

        /** Parent of this view, can be null */
        ::zwlr_foreign_toplevel_handle_v1 * parent = nullptr;
        bool parentChanged = false;

        /** List of outputs from which window has left */
        QList<::wl_output *> outputsLeft;
};

class LXQtTaskbarWlrootsWindow : public QObject,
                                public QtWayland::zwlr_foreign_toplevel_handle_v1
{
    Q_OBJECT
public:
    LXQtTaskbarWlrootsWindow(::zwlr_foreign_toplevel_handle_v1 *id);
    ~LXQtTaskbarWlrootsWindow();

    inline WId getWindowId() const { return reinterpret_cast<WId>(this); }

    void activate();

    QIcon icon;
    WindowProperties windowState;
    WId parentWindow = 0;

Q_SIGNALS:
    void titleChanged();
    void appIdChanged();
    void outputsChanged();

    /** Individual state change signals */
    void maximizedChanged();
    void minimizedChanged();
    void activatedChanged();
    void fullscreenChanged();

    void parentChanged();

    /** Bulk state change signal */
    void stateChanged();

    /** First state change signal: Before this, the window did not have a valid state */
    void windowReady();

    /** All state changes have been sent. */
    void done();

    /** Window closed signal */
    void closed();

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
    void setParentWindow(LXQtTaskbarWlrootsWindow *parent);

    QMetaObject::Connection parentWindowUnmappedConnection;

    WindowProperties m_pendingState;

    bool initDone = false;
};
