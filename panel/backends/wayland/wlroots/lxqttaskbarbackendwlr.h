#pragma once

#include "../../ilxqttaskbarabstractbackend.h"

#include <QTime>
#include <QHash>
#include <vector>

class LXQtTaskBarWlrootsWindow;
class LXQtTaskBarWlrootsWindowManagment;
class LXQtWlrootsWaylandWorkspaceInfo;


class LXQtTaskbarWlrootsBackend : public ILXQtTaskbarAbstractBackend
{
    Q_OBJECT

public:
    explicit LXQtTaskbarWlrootsBackend(QObject *parent = nullptr);

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
    void addWindow(LXQtTaskBarWlrootsWindow *window);
    bool acceptWindow(LXQtTaskBarWlrootsWindow *window) const;

private:
    LXQtTaskBarWlrootsWindow *getWindow(WId windowId) const;

    std::unique_ptr<LXQtWlrootsWaylandWorkspaceInfo> m_workspaceInfo;

    std::unique_ptr<LXQtTaskBarWlrootsWindowManagment> m_managment;

    QHash<LXQtTaskBarWlrootsWindow *, QTime> lastActivated;
    LXQtTaskBarWlrootsWindow *activeWindow = nullptr;
    std::vector<std::unique_ptr<LXQtTaskBarWlrootsWindow>> windows;
    // key=transient child, value=leader
    QHash<LXQtTaskBarWlrootsWindow *, LXQtTaskBarWlrootsWindow *> transients;
    // key=leader, values=transient children
    QMultiHash<LXQtTaskBarWlrootsWindow *, LXQtTaskBarWlrootsWindow *> transientsDemandingAttention;
};
