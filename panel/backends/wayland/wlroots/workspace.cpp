#include "workspace.hpp"

#include <QMap>

/** ext_workspace_handle_v1 <-> WorkspaceHandleV1 map */
QMap<struct ::ext_workspace_handle_v1 *, LXQt::Taskbar::WorkspaceHandleV1 *> workspaceMap;

/**
 * Implementation of the LXQt::TaskBar::WorkspaceManagerV1 class
 */

LXQt::Taskbar::WorkspaceManagerV1::WorkspaceManagerV1() : QWaylandClientExtensionTemplate(
        version ) {
    /** Automatically destroy thie object */
    connect(
        this, &QWaylandClientExtension::activeChanged, this, [ this ]{
            if ( !isActive() ) {
                ext_workspace_manager_v1_destroy( object() );
            }
        } );
}


LXQt::Taskbar::WorkspaceManagerV1::~WorkspaceManagerV1() {
    if ( !isActive() ) {
        ext_workspace_manager_v1_destroy( object() );
    }
}


int LXQt::Taskbar::WorkspaceManagerV1::workspaceCount( QScreen * ) {
    return workspaceMap.count();
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace_group( struct ::ext_workspace_group_handle_v1 *workspace_group ) {
    emit workspaceGroup( new WorkspaceGroupHandleV1( workspace_group ) );
}


void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace( struct ::ext_workspace_handle_v1 *workspace_ ) {
    workspaceMap[ workspace_ ] = new WorkspaceHandleV1( workspace_ );
    emit workspace( workspaceMap[ workspace_ ] );
}


void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_done() {
    emit done();
}


void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_finished() {
    emit finished();
}


/**
 * Implementation of the LXQt::TaskBar::WorkspaceGroupHandleV1 class
 */

LXQt::Taskbar::WorkspaceGroupHandleV1::WorkspaceGroupHandleV1( struct ::ext_workspace_group_handle_v1 *object ): QObject(), QtWayland::ext_workspace_group_handle_v1( object ) {
}


LXQt::Taskbar::WorkspaceGroupHandleV1::~WorkspaceGroupHandleV1() {
    destroy();
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_capabilities( uint32_t caps ) {
    m_supported_capabilities = caps;
    emit capabilities( caps );
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_output_enter( struct ::wl_output *output ) {
    if ( !outputs.contains( output ) ) {
        outputs << output;

        emit outputEnter( output );
    }
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_output_leave( struct ::wl_output *output ) {
    if ( outputs.contains( output ) ) {
        outputs.removeAll( output );

        emit outputLeave( output );
    }
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_enter( struct ::ext_workspace_handle_v1 *workspace ) {
    if ( !workspaces.contains( workspaceMap[ workspace ] ) ) {
        workspaces << workspaceMap[ workspace ];

        emit workspaceAdded( workspaceMap[ workspace ] );
    }
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_leave( struct ::ext_workspace_handle_v1 *workspace ) {
    if ( workspaces.contains( workspaceMap[ workspace ] ) ) {
        workspaces.removeAll( workspaceMap[ workspace ] );

        emit workspaceRemoved( workspaceMap[ workspace ] );
    }
}


void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_removed() {
    emit removed();
}


/**
 * Implementation of the LXQt::TaskBar::WorkspaceHandleV1 class
 */


LXQt::Taskbar::WorkspaceHandleV1::WorkspaceHandleV1( struct ::ext_workspace_handle_v1 *object ): QObject(), QtWayland::ext_workspace_handle_v1( object ) {
}


LXQt::Taskbar::WorkspaceHandleV1::~WorkspaceHandleV1() {
    destroy();
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_id( const QString& id_ ) {
    m_id = id_;

    emit id( m_id );
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_name( const QString& name_ ) {
    m_name = name_;

    emit name( m_name );
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_coordinates( wl_array *coordinates_ ) {
    m_coordinates.clear();

    int32_t *data = static_cast<int32_t *>( coordinates_->data );
    size_t  count = coordinates_->size / sizeof( int32_t );

    for (size_t i = 0; i < count; ++i) {
        m_coordinates.append( data[ i ] );
    }

    emit coordinates( m_coordinates );
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_state( uint32_t state_ ) {
    m_state = state_;

    emit state( m_state );
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_capabilities( uint32_t capabilities_ ) {
    m_capabilities = capabilities_;

    emit capabilities( m_capabilities );
}


void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_removed() {
    if ( workspaceMap.contains( object() ) ) {
        workspaceMap.remove( object() );
    }

    emit removed();
}
