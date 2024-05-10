#ifndef LXQTTASKBARBACKEND_X11_H
#define LXQTTASKBARBACKEND_X11_H

#include "../ilxqttaskbarabstractbackend.h"

//TODO: make PIMPL to forward declare NET::Properties, Display, xcb_connection_t
#include <netwm_def.h>

typedef struct _XDisplay Display;
struct xcb_connection_t;

class LXQtTaskbarX11Backend : public ILXQtTaskbarAbstractBackend
{
    Q_OBJECT

public:
    explicit LXQtTaskbarX11Backend(QObject *parent = nullptr);

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

private slots:
    void onWindowChanged(WId windowId, NET::Properties prop, NET::Properties2 prop2);
    void onWindowAdded(WId windowId);
    void onWindowRemoved(WId windowId);

private:
    bool acceptWindow(WId windowId) const;
    void addWindow_internal(WId windowId, bool emitAdded = true);

private:
    Display *m_X11Display;
    xcb_connection_t *m_xcbConnection;

    QVector<WId> m_windows;
};

#endif // LXQTTASKBARBACKEND_X11_H
