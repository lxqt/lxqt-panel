#include "lxqtwlrvirtualdesktop.h"

#include <algorithm>

LXQtWlrootsVirtualDesktop::LXQtWlrootsVirtualDesktop(const QString &id)
    : id(id)
    , name(QString::fromUtf8("Desktop 1"))
{
}

LXQtWlrootsVirtualDesktop::~LXQtWlrootsVirtualDesktop()
{
}

LXQtWlrootsVirtualDesktopManagment::LXQtWlrootsVirtualDesktopManagment() : QObject()
{
}

LXQtWlrootsVirtualDesktopManagment::~LXQtWlrootsVirtualDesktopManagment()
{
}

LXQtWlrootsWaylandWorkspaceInfo::LXQtWlrootsWaylandWorkspaceInfo()
{
    init();
}

LXQtWlrootsWaylandWorkspaceInfo::VirtualDesktopsIterator LXQtWlrootsWaylandWorkspaceInfo::findDesktop(const QString &id) const
{
    return std::find_if(virtualDesktops.begin(), virtualDesktops.end(),
    [&id](const std::unique_ptr<LXQtWlrootsVirtualDesktop> &desktop) {
        return desktop->id == id;
    });
}

QString LXQtWlrootsWaylandWorkspaceInfo::getDesktopName(int pos) const
{
    if(pos < 0 || size_t(pos) >= virtualDesktops.size())
        return QString();
    return virtualDesktops[pos]->name;
}

QString LXQtWlrootsWaylandWorkspaceInfo::getDesktopId(int pos) const
{
    if(pos < 0 || size_t(pos) >= virtualDesktops.size())
        return QString();
    return virtualDesktops[pos]->id;
}

void LXQtWlrootsWaylandWorkspaceInfo::init()
{
    virtualDesktopManagement = std::make_unique<LXQtWlrootsVirtualDesktopManagment>();
    auto desktopOne = std::make_unique<LXQtWlrootsVirtualDesktop>(QString::fromUtf8("desktop-1"));
    virtualDesktops.insert(std::next(virtualDesktops.begin(), 0), std::move(desktopOne));

    currentVirtualDesktop = QString::fromUtf8( "desktop-1" );
}

void LXQtWlrootsWaylandWorkspaceInfo::addDesktop(const QString &, quint32)
{
}

QVariant LXQtWlrootsWaylandWorkspaceInfo::currentDesktop() const
{
    return currentVirtualDesktop;
}

int LXQtWlrootsWaylandWorkspaceInfo::numberOfDesktops() const
{
    // will always be 1 (at least until ext-workspace becomes available)
    return 1;
}

quint32 LXQtWlrootsWaylandWorkspaceInfo::position(const QVariant &desktop) const
{
    return 0;
}

QVariantList LXQtWlrootsWaylandWorkspaceInfo::desktopIds() const
{
    return { virtualDesktops.at( 0 ).get()->id };
}

QStringList LXQtWlrootsWaylandWorkspaceInfo::desktopNames() const
{
    return { virtualDesktops.at( 0 ).get()->name };
}

int LXQtWlrootsWaylandWorkspaceInfo::desktopLayoutRows() const
{
    return 1;
}

void LXQtWlrootsWaylandWorkspaceInfo::requestActivate(const QVariant &)
{
}

void LXQtWlrootsWaylandWorkspaceInfo::requestCreateDesktop(quint32 position)
{
}

void LXQtWlrootsWaylandWorkspaceInfo::requestRemoveDesktop(quint32 position)
{
}
