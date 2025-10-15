#include "workspace.hpp"

#include <QMap>

#include <private/qwaylandscreen_p.h>

/** ext_workspace_handle_v1 <-> WorkspaceHandleV1 map */
QMap<struct ::ext_workspace_handle_v1*, LXQt::Taskbar::WorkspaceHandleV1*> workspaceMap;
QList<LXQt::Taskbar::WorkspaceGroupHandleV1*> workspaceGroups;

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

int LXQt::Taskbar::WorkspaceManagerV1::workspaceCount(QScreen *screen)
{
    if (screen)
    {
        if (auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(screen->handle()))
        {
            wl_output *output = waylandScreen->output();
            for (auto *wg : std::as_const(workspaceGroups))
            {
                if (wg->outputs().contains(output))
                    return wg->workspaces().count();
            }
        }
    }

    return workspaceMap.count();
}

int LXQt::Taskbar::WorkspaceManagerV1::currentWorkspaceIndex(QScreen *screen)
{
    QMap<QList<int>, WorkspaceHandleV1*> map; // sorted by key
    if (screen)
    {
        if (auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(screen->handle()))
        {
            wl_output *output = waylandScreen->output();
            for (auto *wg : std::as_const(workspaceGroups))
            {
                if (wg->outputs().contains(output))
                {
                    const auto workspaces = wg->workspaces();
                    for (WorkspaceHandleV1 *ws : workspaces)
                    {
                        map[ws->coordinates()] = ws;
                    }
                    break;
                }
            }
        }
    }
    if (!map.isEmpty())
    {
        int index = 0;
        for (WorkspaceHandleV1 *ws : std::as_const(map))
        {
            ++index;
            if (ws->state() & WorkspaceHandleV1::state_active)
            {
                return index;
            }
        }
    }

    return -1;
}

QString LXQt::Taskbar::WorkspaceManagerV1::workspaceName(int idx, const QString &sceenName)
{
    QMap<QList<int>, WorkspaceHandleV1*> map;
    const auto screens = QGuiApplication::screens();
    for (const auto& scr : screens)
    {
        if (scr->name() == sceenName)
        {
            for (auto *wg : std::as_const(workspaceGroups))
            {
                if (wg->screens().contains(scr))
                {
                    const auto workspaces = wg->workspaces();
                    for (WorkspaceHandleV1 *ws : workspaces)
                    {
                        map[ws->coordinates()] = ws;
                    }
                    break;
                }
            }
            break;
        }
    }
    if (idx <= map.size())
    {
        int index = 0;
        for (WorkspaceHandleV1 *ws : std::as_const(map))
        {
            ++index;
            if (index == idx)
            {
                return ws->name();
            }
        }
    }

    return QString();
}

void LXQt::Taskbar::WorkspaceManagerV1::setCurrentWorkspaceIndex(int idx, QScreen *screen)
{
    if (idx <= 0) return;
    QMap<QList<int>, WorkspaceHandleV1*> map; // sorted by key
    if (screen)
    {
        if (auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(screen->handle()))
        {
            wl_output *output = waylandScreen->output();
            for (auto *wg : std::as_const(workspaceGroups))
            {
                if (wg->outputs().contains(output))
                {
                    const auto workspaces = wg->workspaces();
                    for (WorkspaceHandleV1 *ws : workspaces)
                    {
                        map[ws->coordinates()] = ws;
                    }
                    break;
                }
            }
        }
    }
    if (idx <= map.size())
    {
        int index = 0;
        for (WorkspaceHandleV1 *ws : std::as_const(map))
        {
            ++index;
            if (index == idx)
            {
                //qDebug() << ws->name() << "Activating";
                ws->activate();
                return;
            }
        }
    }
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace_group(
    struct ::ext_workspace_group_handle_v1 *workspace_group)
{
    auto wg = new WorkspaceGroupHandleV1(workspace_group);
    if (!workspaceGroups.contains(wg))
    {
        workspaceGroups << wg;
        emit workspaceGroupAdded(wg);
        connect(wg, &WorkspaceGroupHandleV1::workspaceAdded, this, &WorkspaceManagerV1::workspaceAdded);
        connect(wg, &WorkspaceGroupHandleV1::workspaceRemoved, this, &WorkspaceManagerV1::workspaceRemoved);

        connect(this, &WorkspaceManagerV1::activation, wg, [this, wg]() {
                emit currentWorkspaceChanged(wg->screens());
        });
    }
}

void LXQt::Taskbar::WorkspaceManagerV1::ext_workspace_manager_v1_workspace(
    struct ::ext_workspace_handle_v1 *workspace_)
{
    workspaceMap[workspace_] = new WorkspaceHandleV1(workspace_, workspaceMap.count() + 1);

    connect(workspaceMap[workspace_], &WorkspaceHandleV1::activated, this,
            &WorkspaceManagerV1::activation, Qt::QueuedConnection);
    connect(workspaceMap[workspace_], &WorkspaceHandleV1::deactivated, this,
            &WorkspaceManagerV1::activation, Qt::QueuedConnection);
    connect(workspaceMap[workspace_], &WorkspaceHandleV1::nameChanged, this,
            &WorkspaceManagerV1::nameChanged, Qt::QueuedConnection);
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
    if (!m_outputs.contains(output))
    {
        m_outputs << output;
        const auto screens = QGuiApplication::screens();
        for (const auto& scr : screens)
        {
            if (auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(scr->handle()))
            {
                if (output == waylandScreen->output())
                {
                    if (!m_screens.contains(scr))
                        m_screens << scr;
                    break;
                }
            }
        }

        emit outputEnter(output);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_output_leave(
    struct ::wl_output *output)
{
    if (m_outputs.contains(output))
    {
        m_outputs.removeAll(output);
        const auto screens = QGuiApplication::screens();
        for (const auto& scr : screens)
        {
            if (auto waylandScreen = dynamic_cast<QtWaylandClient::QWaylandScreen*>(scr->handle()))
            {
                if (output == waylandScreen->output())
                {
                    m_screens.removeAll(scr);
                    break;
                }
            }
        }
    }

    emit outputLeave(output);
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_enter(
    struct ::ext_workspace_handle_v1 *workspace)
{
    if (!workspaceGroups.contains(this))
    {
        workspaceGroups << this;
    }
    if (workspaceMap.contains(workspace) && !m_workspaces.contains(workspaceMap[workspace]))
    {
        m_workspaces << workspaceMap[workspace];

        emit workspaceAdded(workspaceMap[workspace]);
        //qDebug() << "Workspace added";
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_workspace_leave(
    struct ::ext_workspace_handle_v1 *workspace)
{
    if (workspaceMap.contains(workspace) && m_workspaces.contains(workspaceMap[workspace]))
    {
        m_workspaces.removeAll(workspaceMap[workspace]);
        if (m_workspaces.isEmpty())
        {
            workspaceGroups.removeAll(this);
        }
        emit workspaceRemoved(workspaceMap[workspace]);
    }
}

void LXQt::Taskbar::WorkspaceGroupHandleV1::ext_workspace_group_handle_v1_removed()
{
    workspaceGroups.removeAll(this);
    emit removed();
}

/**
 * Implementation of the LXQt::TaskBar::WorkspaceHandleV1 class
 */


LXQt::Taskbar::WorkspaceHandleV1::WorkspaceHandleV1(struct ::ext_workspace_handle_v1 *object,
    int index) : QObject(),
    QtWayland::ext_workspace_handle_v1(object)
{
    m_coordinates << index;
}

LXQt::Taskbar::WorkspaceHandleV1::~WorkspaceHandleV1()
{
    destroy();
}

QString LXQt::Taskbar::WorkspaceHandleV1::id() const
{
    return m_id;
}

QString LXQt::Taskbar::WorkspaceHandleV1::name() const
{
    return m_name;
}

QList<int> LXQt::Taskbar::WorkspaceHandleV1::coordinates() const
{
    return m_coordinates;
}

uint32_t LXQt::Taskbar::WorkspaceHandleV1::state() const
{
    return m_state;
}

uint32_t LXQt::Taskbar::WorkspaceHandleV1::capabilities() const
{
    return m_capabilities;
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_id(const QString& id_)
{
    m_id = id_;
    //qDebug() << "ID:" << m_id;

    emit idChanged(m_id);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_name(const QString& name_)
{
    m_name = name_;
    //qDebug() << "Name:" << m_name;

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

    //qDebug() << "Coords:" << m_name << m_coordinates;

    emit coordinatesChanged(m_coordinates);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_state(uint32_t state_)
{
    auto old_state = m_state;
    m_state = state_;
    /** Check if this workspace was activated */
    if (!(old_state & state_active) && (state_ & state_active))
    {
        emit activated();
    }
    /** Check if this workspace was deactivated */
    else if ((old_state & state_active) && !(state_ & state_active))
    {
        emit deactivated();
    }

    //qDebug() << "State:" << m_name << m_state;

    emit stateChanged(m_state);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_capabilities(uint32_t capabilities_)
{
    m_capabilities = capabilities_;
    //qDebug() << "Caps:" << m_name << m_capabilities;

    emit capabilitiesChanged(m_capabilities);
}

void LXQt::Taskbar::WorkspaceHandleV1::ext_workspace_handle_v1_removed()
{
    if (auto key = workspaceMap.key(this))
    {
        workspaceMap.remove(key);
    }
    emit removed();
}
