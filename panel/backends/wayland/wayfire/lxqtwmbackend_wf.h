#pragma once

#include "../../ilxqtabstractwmiface.h"
#include "wayfire-common.h"

#include <QTime>
#include <QHash>
#include <vector>

class LXQtTaskbarWayfireWindow;
class LXQtTaskbarWayfireWindowManagment;
class LXQtWayfireWaylandWorkspaceInfo;


class LXQtTaskbarWayfireBackend : public ILXQtAbstractWMInterface
{
    Q_OBJECT

  public:
    explicit LXQtTaskbarWayfireBackend(QObject *parent = nullptr);

    // Backend
    virtual bool supportsAction(WId windowId, LXQtTaskBarBackendAction action) const override;

    // Windows
    virtual bool reloadWindows() override;

    // Get the current windows
    virtual QVector<WId> getCurrentWindows() const override;

    // Get the window title
    virtual QString getWindowTitle(WId windowId) const override;

    // We do not support this
    virtual bool applicationDemandsAttention(WId windowId) const override;

    // Support for this is based on app-id (handled by LXQt)
    virtual QIcon getApplicationIcon(WId windowId, int devicePixels) const override;

    // Same as app-id
    virtual QString getWindowClass(WId windowId) const override;

    // Allways-on-Bottom, Normal or Always-on-top.
    // Always-on-bottom is not available on wayfire
    virtual LXQtTaskBarWindowLayer getWindowLayer(WId windowId) const override;
    virtual bool setWindowLayer(WId windowId, LXQtTaskBarWindowLayer layer) override;

    // Hidden, FullScreen, Minimized, Maximized, MaximizedVertical, MaximizedHorizontally, Normal, RolledUp
    virtual LXQtTaskBarWindowState getWindowState(WId windowId) const override;
    virtual bool setWindowState(WId windowId, LXQtTaskBarWindowState state, bool set) override;

    // Is window active
    virtual bool isWindowActive(WId windowId) const override;

    // Set window as active
    virtual bool raiseWindow(WId windowId, bool onCurrentWorkSpace) override;

    // Close window
    virtual bool closeWindow(WId windowId) override;

    // Get active window
    virtual WId getActiveWindow() const override;

    // Workspaces
    virtual int getWorkspacesCount() const override;
    virtual QString getWorkspaceName(int idx, QString outputName = QString()) const override;

    // Get/Set the current workspace
    virtual int getCurrentWorkspace() const override;
    virtual bool setCurrentWorkspace(int idx) override;

    // Get/Set the workspace of a window
    virtual int getWindowWorkspace(WId windowId) const override;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) override;

    // Move window to previous/next desktop
    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next,
        bool raiseOnCurrentDesktop) override;

    virtual bool isWindowOnScreen(QScreen *screen, WId windowId) const override;

    // Not supported on wayfire at the moment
    virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft);

    // X11 Specific
    virtual void moveApplication(WId windowId) override;
    virtual void resizeApplication(WId windowId) override;

    // ???
    virtual void refreshIconGeometry(WId windowId, const QRect & geom) override;

    // Panel internal - not supported
    virtual bool isAreaOverlapped(const QRect& area) const override;

    // Show Desktop
    virtual bool isShowingDesktop() const override;
    virtual bool showDesktop(bool value) override;

  private:
    // std::unique_ptr<LXQtTaskbarWayfireWindowManagment> mManagment;
    QScopedPointer<LXQt::Panel::Wayfire> mWayfire;

    // Hash-map of view ids, vs their properties
    QHash<WaylandId, QJsonObject> mViews;

    // key=transient child, value=leader
    QHash<WaylandId, WaylandId> transients;


    // Is Desktop Shown
    bool mIsDesktopShowing = false;
};


class LXQtWMBackendWayfireLibrary : public QObject, public ILXQtWMBackendLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/WMInterface/1.0")
    Q_INTERFACES(ILXQtWMBackendLibrary)

  public:
    int getBackendScore(const QString& key) const override;

    ILXQtAbstractWMInterface * instance() const override;
};
