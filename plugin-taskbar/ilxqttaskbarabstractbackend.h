#ifndef ILXQTTASKBARABSTRACTBACKEND_H
#define ILXQTTASKBARABSTRACTBACKEND_H

#include <QObject>

#include "lxqttaskbartypes.h"

class QIcon;
class QScreen;

class ILXQtTaskbarAbstractBackend : public QObject
{
    Q_OBJECT

public:
    explicit ILXQtTaskbarAbstractBackend(QObject *parent = nullptr);

    // Backend
    virtual bool supportsAction(WId windowId, LXQtTaskBarBackendAction action) const = 0;

    // Windows
    virtual bool reloadWindows() = 0;

    virtual QVector<WId> getCurrentWindows() const = 0;

    virtual QString getWindowTitle(WId windowId) const = 0;

    virtual bool applicationDemandsAttention(WId windowId) const = 0;

    virtual QIcon getApplicationIcon(WId windowId, int fallbackDevicePixels) const = 0;

    virtual QString getWindowClass(WId windowId) const = 0;

    virtual LXQtTaskBarWindowLayer getWindowLayer(WId windowId) const = 0;
    virtual bool setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer) = 0;

    virtual LXQtTaskBarWindowState getWindowState(WId windowId) const = 0;
    virtual bool setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set = true) = 0;

    virtual bool isWindowActive(WId windowId) const = 0;
    virtual bool raiseWindow(WId windowId, bool onCurrentWorkSpace) = 0;

    virtual bool closeWindow(WId windowId) = 0;

    virtual WId getActiveWindow() const = 0;

    // Workspaces
    virtual int getWorkspacesCount() const = 0;
    virtual QString getWorkspaceName(int idx) const = 0;

    virtual int getCurrentWorkspace() const = 0;
    virtual bool setCurrentWorkspace(int idx) = 0;

    virtual int getWindowWorkspace(WId windowId) const = 0;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) = 0;

    virtual void moveApplicationToPrevNextDesktop(WId windowId, bool next); // Default implementation
    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) = 0;

    virtual bool isWindowOnScreen(QScreen *screen, WId windowId) const = 0;

    // X11 Specific
    virtual void moveApplication(WId windowId) = 0;
    virtual void resizeApplication(WId windowId) = 0;

    virtual void refreshIconGeometry(WId windowId, const QRect &geom) = 0;

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

#endif // ILXQTTASKBARABSTRACTBACKEND_H
