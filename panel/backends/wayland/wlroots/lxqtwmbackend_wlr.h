#pragma once

#include "../../ilxqtabstractwmiface.h"

#include <QTime>
#include <QHash>
#include <vector>

class LXQtTaskbarWlrootsWindow;
class LXQtTaskbarWlrootsWindowManagment;
class LXQtWlrootsWaylandWorkspaceInfo;


class LXQtTaskbarWlrootsBackend : public ILXQtAbstractWMInterface
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
    virtual QString getWorkspaceName(int idx, QString sceenName = QString()) const override;

    virtual int getCurrentWorkspace() const override;
    virtual bool setCurrentWorkspace(int idx) override;

    virtual int getWindowWorkspace(WId windowId) const override;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) override;

    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

    virtual bool isWindowOnScreen(QScreen *screen, WId windowId) const override;

    virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft);

    // X11 Specific
    virtual void moveApplication(WId windowId) override;
    virtual void resizeApplication(WId windowId) override;

    virtual void refreshIconGeometry(WId windowId, const QRect &geom) override;

    // Panel internal
    virtual bool isAreaOverlapped(const QRect& area) const override;

    // Show Destop
    virtual bool isShowingDesktop() const override;
    virtual bool showDesktop(bool value) override;

private slots:
    void addWindow(WId wid);
    void removeWindow();
    void removeTransient();
    void onActivatedChanged();
    void onParentChanged();
    void onTitleChanged();
    void onAppIdChanged();
    void onStateChanged();

private:
    void addToWindows(WId winId);
    bool acceptWindow(WId wid) const;
    WId findWindow(WId tgt) const;
    WId findTopParent(WId winId) const;
    bool equalIds(WId windowId1, WId windowId2) const;

    /** Convert WId (i.e. quintptr into LXQtTaskbarWlrootsWindow*) */
    LXQtTaskbarWlrootsWindow *getWindow(WId windowId) const;

    std::unique_ptr<LXQtTaskbarWlrootsWindowManagment> m_managment;

    QHash<WId, QTime> lastActivated;
    WId activeWindow = 0;
    std::vector<WId> windows;

    // key=transient child, value=leader
    QHash<WId, WId> transients;
};


class LXQtWMBackendWlrootsLibrary: public QObject, public ILXQtWMBackendLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/WMInterface/1.0")
    Q_INTERFACES(ILXQtWMBackendLibrary)
public:
    int getBackendScore(const QString& key) const override;

    ILXQtAbstractWMInterface* instance() const override;
};
