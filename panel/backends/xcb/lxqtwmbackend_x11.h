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


#ifndef LXQT_WM_BACKEND_X11_H
#define LXQT_WM_BACKEND_X11_H

#include "../ilxqtabstractwmiface.h"

#include <netwm_def.h>

typedef struct _XDisplay Display;
struct xcb_connection_t;

class LXQtWMBackendX11 : public ILXQtAbstractWMInterface
{
    Q_OBJECT

public:
    explicit LXQtWMBackendX11(QObject *parent = nullptr);

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
    virtual QString getWorkspaceName(int idx, QString screenName = QString()) const override;

    virtual int getCurrentWorkspace() const override;
    virtual bool setCurrentWorkspace(int idx) override;

    virtual int getWindowWorkspace(WId windowId) const override;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) override;

    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) override;

    virtual int onAllWorkspacesEnum() const override;

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
    void addWindow_internal(WId windowId);

private:
    Display *m_X11Display;
    xcb_connection_t *m_xcbConnection;

    QVector<WId> m_windows;
};

class LXQtWMBackendX11Library: public QObject, public ILXQtWMBackendLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/WMInterface/1.0")
    Q_INTERFACES(ILXQtWMBackendLibrary)
public:
    int getBackendScore(const QString& key) const override;

    ILXQtAbstractWMInterface* instance() const override;
};

#endif // LXQT_WM_BACKEND_X11_H
