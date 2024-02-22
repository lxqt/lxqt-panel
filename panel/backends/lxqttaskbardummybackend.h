#ifndef LXQTTASKBARDUMMYBACKEND_H
#define LXQTTASKBARDUMMYBACKEND_H

#include "ilxqttaskbarabstractbackend.h"

class LXQtTaskBarDummyBackend : public ILXQtTaskbarAbstractBackend
{
    Q_OBJECT

public:
    explicit LXQtTaskBarDummyBackend(QObject *parent = nullptr);

    // Backend
    bool supportsAction(WId windowId, LXQtTaskBarBackendAction action) const override;

    // Windows
    bool reloadWindows() override;

    QVector<WId> getCurrentWindows() const override;

    QString getWindowTitle(WId windowId) const override;

    bool applicationDemandsAttention(WId windowId) const override;

    QIcon getApplicationIcon(WId windowId, int fallbackDevicePixels) const override;

    QString getWindowClass(WId windowId) const override;

    LXQtTaskBarWindowLayer getWindowLayer(WId windowId) const override;
    bool setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer) override;

    LXQtTaskBarWindowState getWindowState(WId windowId) const override;
    bool setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set = true) override;

    bool isWindowActive(WId windowId) const override;
    bool raiseWindow(WId windowId, bool onCurrentWorkSpace) override;

    bool closeWindow(WId windowId) override;

    WId getActiveWindow() const override;

    // Workspaces
    int getWorkspacesCount() const override;
    QString getWorkspaceName(int idx) const override;

    int getCurrentWorkspace() const override;
    bool setCurrentWorkspace(int idx) override;

    int getWindowWorkspace(WId windowId) const override;
    bool setWindowOnWorkspace(WId windowId, int idx) override;

    void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

    bool isWindowOnScreen(QScreen *screen, WId windowId) const override;

    // X11 Specific
    void moveApplication(WId windowId) override;
    void resizeApplication(WId windowId) override;

    void refreshIconGeometry(WId windowId, const QRect &geom) override;

    // Panel internal
    bool isAreaOverlapped(const QRect& area) const override;

    // Show Destop
    bool isShowingDesktop() const override;
    bool showDesktop(bool value) override;

signals:
    void reloaded();

    // Windows
    void windowAdded(WId windowId);
    void windowRemoved(WId windowId);
    void windowPropertyChanged(WId windowId, int prop);

    // Workspaces
    void workspacesCountChanged();
    void workspaceNameChanged(int idx);
    void currentWorkspaceChanged(int idx);

    // TODO: needed?
    void activeWindowChanged(WId windowId);
};

#endif // LXQTTASKBARDUMMYBACKEND_H
