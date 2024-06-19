/*
    SPDX-FileCopyrightText: 2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

    Adapted from KDE Plasma Workspace: plasma-workspace/libtaskmanager/waylandtasksmodel.cpp
*/


#include "lxqtplasmavirtualdesktop.h"

#include <algorithm>

LXQtPlasmaVirtualDesktop::LXQtPlasmaVirtualDesktop(::org_kde_plasma_virtual_desktop *object, const QString &id)
    : org_kde_plasma_virtual_desktop(object)
    , id(id)
{
}

LXQtPlasmaVirtualDesktop::~LXQtPlasmaVirtualDesktop()
{
    wl_proxy_destroy(reinterpret_cast<wl_proxy *>(object()));
}

void LXQtPlasmaVirtualDesktop::org_kde_plasma_virtual_desktop_name(const QString &name)
{
    this->name = name;
    Q_EMIT nameChanged();
}

void LXQtPlasmaVirtualDesktop::org_kde_plasma_virtual_desktop_done()
{
    Q_EMIT done();
}

void LXQtPlasmaVirtualDesktop::org_kde_plasma_virtual_desktop_activated()
{
    Q_EMIT activated();
}

LXQtPlasmaVirtualDesktopManagment::LXQtPlasmaVirtualDesktopManagment()
    : QWaylandClientExtensionTemplate(version)
{
    connect(this, &QWaylandClientExtension::activeChanged, this, [this] {
        if (!isActive()) {
            org_kde_plasma_virtual_desktop_management_destroy(object());
        }
    });
}

LXQtPlasmaVirtualDesktopManagment::~LXQtPlasmaVirtualDesktopManagment()
{
    if (isActive()) {
        org_kde_plasma_virtual_desktop_management_destroy(object());
    }
}

void LXQtPlasmaVirtualDesktopManagment::org_kde_plasma_virtual_desktop_management_desktop_created(const QString &desktop_id, uint32_t position)
{
    emit desktopCreated(desktop_id, position);
}

void LXQtPlasmaVirtualDesktopManagment::org_kde_plasma_virtual_desktop_management_desktop_removed(const QString &desktop_id)
{
    emit desktopRemoved(desktop_id);
}

void LXQtPlasmaVirtualDesktopManagment::org_kde_plasma_virtual_desktop_management_rows(uint32_t rows)
{
    emit rowsChanged(rows);
}

LXQtPlasmaWaylandWorkspaceInfo::LXQtPlasmaWaylandWorkspaceInfo()
{
    init();
}

LXQtPlasmaWaylandWorkspaceInfo::VirtualDesktopsIterator LXQtPlasmaWaylandWorkspaceInfo::findDesktop(const QString &id) const
{
    return std::find_if(virtualDesktops.begin(), virtualDesktops.end(),
    [&id](const std::unique_ptr<LXQtPlasmaVirtualDesktop> &desktop) {
        return desktop->id == id;
    });
}

QString LXQtPlasmaWaylandWorkspaceInfo::getDesktopName(int pos) const
{
    if(pos < 0 || size_t(pos) >= virtualDesktops.size())
        return QString();
    return virtualDesktops[pos]->name;
}

QString LXQtPlasmaWaylandWorkspaceInfo::getDesktopId(int pos) const
{
    if(pos < 0 || size_t(pos) >= virtualDesktops.size())
        return QString();
    return virtualDesktops[pos]->id;
}

void LXQtPlasmaWaylandWorkspaceInfo::init()
{
    virtualDesktopManagement = std::make_unique<LXQtPlasmaVirtualDesktopManagment>();

    connect(virtualDesktopManagement.get(), &LXQtPlasmaVirtualDesktopManagment::activeChanged, this, [this] {
        if (!virtualDesktopManagement->isActive()) {
            rows = 0;
            virtualDesktops.clear();
            currentVirtualDesktop.clear();
            Q_EMIT currentDesktopChanged();
            Q_EMIT numberOfDesktopsChanged();
            Q_EMIT navigationWrappingAroundChanged();
            Q_EMIT desktopIdsChanged();
            Q_EMIT desktopLayoutRowsChanged();
        }
    });

    connect(virtualDesktopManagement.get(), &LXQtPlasmaVirtualDesktopManagment::desktopCreated,
            this, &LXQtPlasmaWaylandWorkspaceInfo::addDesktop);

    connect(virtualDesktopManagement.get(), &LXQtPlasmaVirtualDesktopManagment::desktopRemoved, this, [this](const QString &id) {


        virtualDesktops.erase(std::remove_if(virtualDesktops.begin(), virtualDesktops.end(),
                                             [id](const std::unique_ptr<LXQtPlasmaVirtualDesktop> &desktop)
                                             {
                                                 return desktop->id == id;
                                             }),
                              virtualDesktops.end());

        Q_EMIT numberOfDesktopsChanged();
        Q_EMIT desktopIdsChanged();

        if (currentVirtualDesktop == id) {
            currentVirtualDesktop.clear();
            Q_EMIT currentDesktopChanged();
        }
    });

    connect(virtualDesktopManagement.get(), &LXQtPlasmaVirtualDesktopManagment::rowsChanged, this, [this](quint32 rows) {
        this->rows = rows;
        Q_EMIT desktopLayoutRowsChanged();
    });
}

void LXQtPlasmaWaylandWorkspaceInfo::addDesktop(const QString &id, quint32 pos)
{
    if (findDesktop(id) != virtualDesktops.end()) {
        return;
    }

    auto desktop = std::make_unique<LXQtPlasmaVirtualDesktop>(virtualDesktopManagement->get_virtual_desktop(id), id);

    connect(desktop.get(), &LXQtPlasmaVirtualDesktop::activated, this, [id, this]() {
        currentVirtualDesktop = id;
        Q_EMIT currentDesktopChanged();
    });

    connect(desktop.get(), &LXQtPlasmaVirtualDesktop::nameChanged, this, [id, this]() {
        Q_EMIT desktopNameChanged(position(id));
    });

    connect(desktop.get(), &LXQtPlasmaVirtualDesktop::done, this, [id, this]() {
        Q_EMIT desktopNameChanged(position(id));
    });

    virtualDesktops.insert(std::next(virtualDesktops.begin(), pos), std::move(desktop));

    Q_EMIT numberOfDesktopsChanged();
    Q_EMIT desktopIdsChanged();
    Q_EMIT desktopNameChanged(position(id));
}

QVariant LXQtPlasmaWaylandWorkspaceInfo::currentDesktop() const
{
    return currentVirtualDesktop;
}

int LXQtPlasmaWaylandWorkspaceInfo::numberOfDesktops() const
{
    return virtualDesktops.size();
}

quint32 LXQtPlasmaWaylandWorkspaceInfo::position(const QVariant &desktop) const
{
    return std::distance(virtualDesktops.begin(), findDesktop(desktop.toString()));
}

QVariantList LXQtPlasmaWaylandWorkspaceInfo::desktopIds() const
{
    QVariantList ids;
    ids.reserve(virtualDesktops.size());

    std::transform(virtualDesktops.cbegin(), virtualDesktops.cend(), std::back_inserter(ids), [](const std::unique_ptr<LXQtPlasmaVirtualDesktop> &desktop) {
        return desktop->id;
    });
    return ids;
}

QStringList LXQtPlasmaWaylandWorkspaceInfo::desktopNames() const
{
    if (!virtualDesktopManagement->isActive()) {
        return QStringList();
    }
    QStringList names;
    names.reserve(virtualDesktops.size());

    std::transform(virtualDesktops.cbegin(), virtualDesktops.cend(), std::back_inserter(names), [](const std::unique_ptr<LXQtPlasmaVirtualDesktop> &desktop) {
        return desktop->name;
    });
    return names;
}

int LXQtPlasmaWaylandWorkspaceInfo::desktopLayoutRows() const
{
    if (!virtualDesktopManagement->isActive()) {
        return 0;
    }

    return rows;
}

void LXQtPlasmaWaylandWorkspaceInfo::requestActivate(const QVariant &desktop)
{
    if (!virtualDesktopManagement->isActive()) {
        return;
    }

    if (auto it = findDesktop(desktop.toString()); it != virtualDesktops.end()) {
        (*it)->request_activate();
    }
}

void LXQtPlasmaWaylandWorkspaceInfo::requestCreateDesktop(quint32 position)
{
    if (!virtualDesktopManagement->isActive()) {
        return;
    }

    //TODO: translatestd
    virtualDesktopManagement->request_create_virtual_desktop(QLatin1String("New Desktop"), position);
}

void LXQtPlasmaWaylandWorkspaceInfo::requestRemoveDesktop(quint32 position)
{
    if (!virtualDesktopManagement->isActive()) {
        return;
    }
    if (virtualDesktops.size() == 1) {
        return;
    }

    if (position > (virtualDesktops.size() - 1)) {
        return;
    }

    virtualDesktopManagement->request_remove_virtual_desktop(virtualDesktops.at(position)->id);
}

