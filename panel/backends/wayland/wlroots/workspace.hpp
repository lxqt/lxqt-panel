#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QScreen>

#include <string>
#include "wayland-ext-workspace-v1-client-protocol.h"
#include "qwayland-ext-workspace-v1.h"

#include <QWaylandClientExtension>

struct wl_registry;

namespace LXQt {
    namespace Taskbar {
        class WorkspaceManagerV1;
        class WorkspaceGroupHandleV1;
        class WorkspaceHandleV1;
    }
}

class LXQt::Taskbar::WorkspaceManagerV1 : public QWaylandClientExtensionTemplate<LXQt::Taskbar::WorkspaceManagerV1>,
                                          public QtWayland::ext_workspace_manager_v1 {
    Q_OBJECT

    public:
        static constexpr int version = 16;

        WorkspaceManagerV1();
        virtual ~WorkspaceManagerV1();

        /**
         * TODO: We have to eventually implement this.
         */
        int workspaceCount( QScreen *screen = nullptr );

        Q_SIGNAL void workspaceGroup( WorkspaceGroupHandleV1 *workspace_group );
        Q_SIGNAL void workspace( WorkspaceHandleV1 *workspace );
        Q_SIGNAL void done();
        Q_SIGNAL void finished();

    protected:
        virtual void ext_workspace_manager_v1_workspace_group( struct ::ext_workspace_group_handle_v1 *workspace_group );
        virtual void ext_workspace_manager_v1_workspace( struct ::ext_workspace_handle_v1 *workspace );
        virtual void ext_workspace_manager_v1_done();
        virtual void ext_workspace_manager_v1_finished();
};

class LXQt::Taskbar::WorkspaceGroupHandleV1 : public QObject, public QtWayland::ext_workspace_group_handle_v1 {
    Q_OBJECT;
    public:
        WorkspaceGroupHandleV1( struct ::ext_workspace_group_handle_v1 *object );
        virtual ~WorkspaceGroupHandleV1();

        /**
         * Note: QtWayland::ext_workspace_group_handle_v1 has following member functions:
         *  - create_workspace( QString )
         *  - destroy()
         *  - object() -> struct ::ext_workspace_group_handle_v1 *
         * These functions are complete, and we do not have to re-implement them here.
         */

        Q_SIGNAL void capabilities( uint32_t capabilities );
        Q_SIGNAL void outputEnter( struct ::wl_output *output );
        Q_SIGNAL void outputLeave( struct ::wl_output *output );
        Q_SIGNAL void workspaceAdded( WorkspaceHandleV1 *workspace );
        Q_SIGNAL void workspaceRemoved( WorkspaceHandleV1 *workspace );
        Q_SIGNAL void removed();

    protected:
        virtual void ext_workspace_group_handle_v1_capabilities( uint32_t capabilities );
        virtual void ext_workspace_group_handle_v1_output_enter( struct ::wl_output *output );
        virtual void ext_workspace_group_handle_v1_output_leave( struct ::wl_output *output );
        virtual void ext_workspace_group_handle_v1_workspace_enter( struct ::ext_workspace_handle_v1 *workspace );
        virtual void ext_workspace_group_handle_v1_workspace_leave( struct ::ext_workspace_handle_v1 *workspace );
        virtual void ext_workspace_group_handle_v1_removed();

    private:
        /** Track on which outputs this workspace group is visible */
        QList<struct ::wl_output *> outputs;

        /** Store the capabilities */
        uint32_t m_supported_capabilities;

        /** Track on which workspaces are a part of this workspace group */
        QList<WorkspaceHandleV1 *> workspaces;
};

class LXQt::Taskbar::WorkspaceHandleV1 : public QObject, public QtWayland::ext_workspace_handle_v1 {
    Q_OBJECT;
    public:
        WorkspaceHandleV1( struct ::ext_workspace_handle_v1 *object );
        virtual ~WorkspaceHandleV1();

        /**
         * Note: QtWayland::ext_workspace_group_handle_v1 has following member functions:
         *  - object() -> struct ::ext_workspace_group_handle_v1 *
         *  - destroy();
         *  - activate();
         *  - deactivate();
         *  - assign(struct ::ext_workspace_group_handle_v1 *workspace_group);
         *  - remove();
         * These functions are complete, and we do not have to re-implement them here.
         */

        QString getId() const;
        QString getName() const;
        QList<int> getCoordinates() const;
        uint32_t getState() const;
        uint32_t getCapabilities() const;

        Q_SIGNAL void id( const QString& id );
        Q_SIGNAL void name( const QString& name );
        Q_SIGNAL void coordinates( QList<int> coordinates );
        Q_SIGNAL void state( uint32_t state );
        Q_SIGNAL void capabilities( uint32_t capabilities );
        Q_SIGNAL void removed();

    protected:
        virtual void ext_workspace_handle_v1_id( const QString& id );
        virtual void ext_workspace_handle_v1_name( const QString& name );
        virtual void ext_workspace_handle_v1_coordinates( wl_array *coordinates );
        virtual void ext_workspace_handle_v1_state( uint32_t state );
        virtual void ext_workspace_handle_v1_capabilities( uint32_t capabilities );
        virtual void ext_workspace_handle_v1_removed();

    private:
        QString m_id;
        QString m_name;
        QList<int> m_coordinates;
        uint32_t m_state;
        uint32_t m_capabilities;
};
