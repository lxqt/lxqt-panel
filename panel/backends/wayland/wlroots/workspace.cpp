#include "workspace.hpp"

#include <QMap>

/** ext_workspace_handle_v1 <-> WorkspaceHandleV1 map */
QMap<struct ::ext_workspace_handle_v1*, LXQt::Taskbar::WorkspaceHandleV1*> workspaceMap;

/**
 * Implementation of the LXQt::TaskBar::WorkspaceManagerV1 class
 */

LXQt::Taskbar::WorkspaceManagerV1::WorkspaceManagerV1() : QWaylandClientExtensionTemplate(
        version)
{
    /** Automatically destroy thie object */
    connect(
        this, &QWaylandClientExtension::activeChanged, this, [this]
    {
        if (!isActive())
        {
            ext_workspace_manager_v1_destroy(object());
        }
    });
}

LXQt::Taskbar::WorkspaceManagerV1::~WorkspaceManagerV1()
{
    if (!isActive())
    {
        ext_workspace_manager_v1_destroy(object());
    }
}

int LXQt::Taskbar::WorkspaceManagerV1::workspaceCount(QScreen*)
{
    return workspaceMap.count();
}

int LXQt::Taskbar::WorkspaceManagerV1::currentWorkspaceIndex(QScreen*)
{
    for ( WorkspaceHandleV1 *ws : workspaceMap.values())
    {
        if (ws->getState() && WorkspaceHandleV1::state_active)
        {
            return ws->getIndex();
        }
    }

    return -1;
}

void LXQt::Taskbar::WorkspaceManagerV1::setCurrentWorkspaceIndex(int idx)
{
    for ( WorkspaceHandleV1 *ws : workspaceMap.values())
    {
        if (ws->getIndex() == idx)
        {
            ws->activate();
        }
    }
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace_group(
    struct ::ext_workspace_group_handle_v1 *workspace_group)
{
    emit workspaceGroupAdded(new WorkspaceGroupHandleV1(workspace_group));
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace(
    struct ::ext_workspace_handle_v1 *workspace_)
{
    workspaceMap[workspace_] = new WorkspaceHandleV1(workspace_);
    emit workspaceAdded(workspaceMap[workspace_]);

    /** Automatically destroy thie object */
    connect(workspaceMap[workspace_], &WorkspaceHandleV1::activated, this,
        &WorkspaceManagerV1::currentWorkspaceChanged);
    connect(workspaceMap[workspace_], &WorkspaceHandleV1::deactivated, this,
        &WorkspaceManagerV1::currentWorkspaceChanged);
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_done()
{
    emit done();
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_finished()
{
    emit finished();
}

/**
 * Implementation of the LXQt::TaskBar::WorkspaceGroupHandleV1 class
 */

LXQt::Taskbar::WorkspaceGroupHandleV1::WorkspaceGroupHandleV1(
    struct ::ext_workspace_group_handle_v1 *object) : QObject(),
    QtWayland::ext_workspace_group_handle_v1(object)
{}

LXQt::Taskbar::WorkspaceGroupHandleV1::~WorkspaceGroupHandleV1()
{
    destroy();
}

bool LXQt::Taskbar::WorkspaceGroupHandleV1::canCreateWorkspace() const
{
    return (m_supported_capabilities & group_capabilities_create_workspace);
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_capabilities(uint32_t caps)
{
    m_supported_capabilities = caps;
    emit capabilities(caps);
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_output_enter(
    struct ::wl_output *output)
{
    if (!outputs.contains(output))
    {
        outputs << output;

        emit outputEnter(output);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_output_leave(
    struct ::wl_output *output)
{
    if (outputs.contains(output))
    {
        outputs.removeAll(output);

        emit outputLeave(output);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_enter(
    struct ::ext_workspace_handle_v1 *workspace)
{
    if (!workspaces.contains(workspaceMap[workspace]))
    {
        workspaces << workspaceMap[workspace];

        emit workspaceAdded(workspaceMap[workspace]);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_leave(
    struct ::ext_workspace_handle_v1 *workspace)
{
    if (workspaces.contains(workspaceMap[workspace]))
    {
        workspaces.removeAll(workspaceMap[workspace]);

        emit workspaceRemoved(workspaceMap[workspace]);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_removed()
{
    emit removed();
}

/**
 * Implementation of the LXQt::TaskBar::WorkspaceHandleV1 class
 */


LXQt::Taskbar::WorkspaceHandleV1::WorkspaceHandleV1(struct ::ext_workspace_handle_v1 *object) : QObject(),
    QtWayland::ext_workspace_handle_v1(object)
{}

LXQt::Taskbar::WorkspaceHandleV1::~WorkspaceHandleV1()
{
    destroy();
}

QString LXQt::Taskbar::WorkspaceHandleV1::getId() const
{
    return m_id;
}

QString LXQt::Taskbar::WorkspaceHandleV1::getName() const
{
    return m_name;
}

QList<int> LXQt::Taskbar::WorkspaceHandleV1::getCoordinates() const
{
    return m_coordinates;
}

uint32_t LXQt::Taskbar::WorkspaceHandleV1::getState() const
{
    return m_state;
}

uint32_t LXQt::Taskbar::WorkspaceHandleV1::getCapabilities() const
{
    return m_capabilities;
}

int LXQt::Taskbar::WorkspaceHandleV1::getIndex() const
{
    return 0;
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_id(const QString& id_)
{
    m_id = id_;

    emit idChanged(m_id);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_name(const QString& name_)
{
    m_name = name_;

    emit nameChanged(m_name);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_coordinates(wl_array *coordinates_)
{
    m_coordinates.clear();

    int32_t *data = static_cast<int32_t*>(coordinates_->data);
    size_t count  = coordinates_->size / sizeof(int32_t);

    for (size_t i = 0; i < count; ++i)
    {
        m_coordinates.append(data[i]);
    }

    emit coordinatesChanged(m_coordinates);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_state(uint32_t state_)
{
    /** Check if this workspace was activated */
    if (!(m_state & state_active) && (state_ & state_active))
    {
        emit activated();
    }

    /** Check if this workspace was deactivated */
    if ((m_state & state_active) && !(state_ & state_active))
    {
        emit deactivated();
    }

    m_state = state_;

    emit stateChanged(m_state);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_capabilities(uint32_t capabilities_)
{
    m_capabilities = capabilities_;

    emit capabilitiesChanged(m_capabilities);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_removed()
{
    if (workspaceMap.contains(object()))
    {
        workspaceMap.remove(object());
    }

    emit removed();
}
