/*
    SPDX-FileCopyrightText: 2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

    Adapted from KDE Plasma Workspace: plasma-workspace/libtaskmanager/waylandtasksmodel.cpp
*/

#ifndef LXQTPLASMAVIRTUALDESKTOP_H
#define LXQTPLASMAVIRTUALDESKTOP_H

#include <QVariant>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include <vector>

#include "qwayland-org-kde-plasma-virtual-desktop.h"

class LXQtPlasmaVirtualDesktop : public QObject, public QtWayland::org_kde_plasma_virtual_desktop
{
    Q_OBJECT
public:
    LXQtPlasmaVirtualDesktop(::org_kde_plasma_virtual_desktop *object, const QString &id);
    ~LXQtPlasmaVirtualDesktop();
    const QString id;
    QString name;
Q_SIGNALS:
    void done();
    void activated();
    void nameChanged();

protected:
    void org_kde_plasma_virtual_desktop_name(const QString &name) override;
    void org_kde_plasma_virtual_desktop_done() override;
    void org_kde_plasma_virtual_desktop_activated() override;
};


class LXQtPlasmaVirtualDesktopManagment : public QWaylandClientExtensionTemplate<LXQtPlasmaVirtualDesktopManagment>,
                                          public QtWayland::org_kde_plasma_virtual_desktop_management
{
    Q_OBJECT
public:
    static constexpr int version = 2;

    LXQtPlasmaVirtualDesktopManagment();
    ~LXQtPlasmaVirtualDesktopManagment();

signals:
    void desktopCreated(const QString &id, quint32 position);
    void desktopRemoved(const QString &id);
    void rowsChanged(const quint32 rows);

protected:
    virtual void org_kde_plasma_virtual_desktop_management_desktop_created(const QString &desktop_id, uint32_t position) override;
    virtual void org_kde_plasma_virtual_desktop_management_desktop_removed(const QString &desktop_id) override;
    virtual void org_kde_plasma_virtual_desktop_management_rows(uint32_t rows) override;
};

class Q_DECL_HIDDEN LXQtPlasmaWaylandWorkspaceInfo : public QObject
{
    Q_OBJECT
public:
    LXQtPlasmaWaylandWorkspaceInfo();

    QVariant currentVirtualDesktop;
    std::vector<std::unique_ptr<LXQtPlasmaVirtualDesktop>> virtualDesktops;
    std::unique_ptr<LXQtPlasmaVirtualDesktopManagment> virtualDesktopManagement;
    quint32 rows;

    typedef std::vector<std::unique_ptr<LXQtPlasmaVirtualDesktop>>::const_iterator VirtualDesktopsIterator;

    VirtualDesktopsIterator findDesktop(const QString &id) const;

    QString getDesktopName(int pos) const;
    QString getDesktopId(int pos) const;

    void init();
    void addDesktop(const QString &id, quint32 pos);
    QVariant currentDesktop() const;
    int numberOfDesktops() const;
    QVariantList desktopIds() const;
    QStringList desktopNames() const;
    quint32 position(const QVariant &desktop) const;
    int desktopLayoutRows() const;
    void requestActivate(const QVariant &desktop);
    void requestCreateDesktop(quint32 position);
    void requestRemoveDesktop(quint32 position);

signals:
    void currentDesktopChanged();
    void numberOfDesktopsChanged();
    void navigationWrappingAroundChanged();
    void desktopIdsChanged();
    void desktopNameChanged(quint32 position);
    void desktopLayoutRowsChanged();
};

#endif // LXQTPLASMAVIRTUALDESKTOP_H
