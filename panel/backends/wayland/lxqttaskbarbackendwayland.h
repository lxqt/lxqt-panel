/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef LXQTTASKBARBACKENDWAYLAND_H
#define LXQTTASKBARBACKENDWAYLAND_H

#include "../ilxqttaskbarabstractbackend.h"

#include <QTime>
#include <QHash>
#include <vector>

class LXQtTaskBarPlasmaWindow;
class LXQtTaskBarPlasmaWindowManagment;
class LXQtPlasmaWaylandWorkspaceInfo;


class LXQtTaskbarWaylandBackend : public ILXQtTaskbarAbstractBackend
{
    Q_OBJECT

public:
    explicit LXQtTaskbarWaylandBackend(QObject *parent = nullptr);

    // Backend
    virtual bool supportsAction(WId windowId, LXQtTaskBarBackendAction action) const override;

    // Windows
    virtual bool reloadWindows() override;

    virtual QVector<WId> getCurrentWindows() const override;
    virtual QString getWindowTitle(WId windowId) const override;
    virtual bool applicationDemandsAttention(WId windowId) const override;
    virtual QIcon getApplicationIcon(WId windowId, int devicePixels) const override;
    virtual QString getWindowClass(WId windowId) const override;

    virtual LXQtTaskBarWindowLayer getWindowLayer(WId windowId) const override;
    virtual bool setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer) override;

    virtual LXQtTaskBarWindowState getWindowState(WId windowId) const override;
    virtual bool setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set) override;

    virtual bool isWindowActive(WId windowId) const override;
    virtual bool raiseWindow(WId windowId, bool onCurrentWorkSpace) override;

    virtual bool closeWindow(WId windowId) override;

    virtual WId getActiveWindow() const override;

    // Workspaces
    virtual int getWorkspacesCount() const override;
    virtual QString getWorkspaceName(int idx) const override;

    virtual int getCurrentWorkspace() const override;
    virtual bool setCurrentWorkspace(int idx) override;

    virtual int getWindowWorkspace(WId windowId) const override;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) override;

    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

    virtual bool isWindowOnScreen(QScreen *screen, WId windowId) const override;

    virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) override;

    // X11 Specific
    virtual void moveApplication(WId windowId) override;
    virtual void resizeApplication(WId windowId) override;

    virtual void refreshIconGeometry(WId windowId, const QRect &geom) override;

    // Panel internal
    virtual bool isAreaOverlapped(const QRect& area) const override;

    // Show Destop
    virtual bool isShowingDesktop() const override;
    virtual bool showDesktop(bool value) override;

private:
    void addWindow(LXQtTaskBarPlasmaWindow *window);
    bool acceptWindow(LXQtTaskBarPlasmaWindow *window) const;
    void updateWindowAcceptance(LXQtTaskBarPlasmaWindow *window);

private:
    LXQtTaskBarPlasmaWindow *getWindow(WId windowId) const;

    std::unique_ptr<LXQtPlasmaWaylandWorkspaceInfo> m_workspaceInfo;

    std::unique_ptr<LXQtTaskBarPlasmaWindowManagment> m_managment;

    QHash<LXQtTaskBarPlasmaWindow *, QTime> lastActivated;
    LXQtTaskBarPlasmaWindow *activeWindow = nullptr;
    std::vector<std::unique_ptr<LXQtTaskBarPlasmaWindow>> windows;
    // key=transient child, value=leader
    QHash<LXQtTaskBarPlasmaWindow *, LXQtTaskBarPlasmaWindow *> transients;
    // key=leader, values=transient children
    QMultiHash<LXQtTaskBarPlasmaWindow *, LXQtTaskBarPlasmaWindow *> transientsDemandingAttention;
};

#endif // LXQTTASKBARBACKENDWAYLAND_H
