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


#ifndef ILXQT_ABSTRACT_WM_INTERFACE_H
#define ILXQT_ABSTRACT_WM_INTERFACE_H

#include <QObject>

#include "../lxqtpanelglobals.h"
#include "lxqttaskbartypes.h"

class QIcon;
class QScreen;

class LXQT_PANEL_API ILXQtAbstractWMInterface : public QObject
{
    Q_OBJECT

public:
    explicit ILXQtAbstractWMInterface(QObject *parent = nullptr);

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
    // NOTE: indexes are 1-based, 0 means "Show on All desktops"
    virtual int getWorkspacesCount() const = 0;
    virtual QString getWorkspaceName(int idx) const = 0;

    virtual int getCurrentWorkspace() const = 0;
    virtual bool setCurrentWorkspace(int idx) = 0;

    virtual int getWindowWorkspace(WId windowId) const = 0;
    virtual bool setWindowOnWorkspace(WId windowId, int idx) = 0;

    virtual void moveApplicationToPrevNextDesktop(WId windowId, bool next); // Default implementation
    virtual void moveApplicationToPrevNextMonitor(WId windowId, bool next, bool raiseOnCurrentDesktop) = 0;

    virtual int onAllWorkspacesEnum() const;

    virtual bool isWindowOnScreen(QScreen *screen, WId windowId) const = 0;

    virtual bool setDesktopLayout(Qt::Orientation orientation, int rows, int columns, bool rightToLeft) = 0;

    // X11 Specific
    virtual void moveApplication(WId windowId) = 0;
    virtual void resizeApplication(WId windowId) = 0;

    virtual void refreshIconGeometry(WId windowId, const QRect &geom) = 0;

    // Panel internal
    virtual bool isAreaOverlapped(const QRect& area) const = 0;

    // Show Destop TODO: split in multiple interfeces, this is becoming big
    // NOTE: KWindowSystem already has these functions
    // However on Wayland they are only compatible with KWin
    // because internally it uses org_kde_plasma_window_management protocol
    // We make this virtual so it can be implemented also for other compositors
    virtual bool isShowingDesktop() const = 0;
    virtual bool showDesktop(bool value) = 0;

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

class LXQT_PANEL_API ILXQtWMBackendLibrary
{
public:
    /**
     Destroys the ILXQtWMBackendLibrary object.
     **/
    virtual ~ILXQtWMBackendLibrary() {}

    /**
    Returns the score of this backend for current detected environment.
    This is used to select correct backend at runtime
     **/
    virtual int getBackendScore(const QString& key) const = 0;

    /**
    Returns the root component object of the backend. When the library is finally unloaded, the root component will automatically be deleted.
     **/
    virtual ILXQtAbstractWMInterface* instance() const = 0;
};


Q_DECLARE_INTERFACE(ILXQtWMBackendLibrary,
                    "lxqt.org/Panel/WMInterface/1.0")

#endif // ILXQT_ABSTRACT_WM_INTERFACE_H
