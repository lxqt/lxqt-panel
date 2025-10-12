/* BEGIN_COMMON_COPYRIGHT_HEADER (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset https://lxqt.org
 *
 * Copyright: 2023 LXQt team Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
 *
 * This program or library is free software; you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#pragma once

/** For struct pollfd */
#include <poll.h>

/* For QString, QThread, QTimer, etc.. */
#include <QtCore>

#include "../../lxqttaskbartypes.h"

namespace LXQt
{
namespace Panel
{
class Wayfire;
class WayfireImpl;
}
}

// Strongly-typed wrapper for Wayland IDs
struct WaylandId
{
    uint32_t id;
    explicit WaylandId(uint32_t id_ = 0) : id(id_)
    {}
    operator WId() const
    {
        return static_cast<WId>(id);
    }
};


class LXQt::Panel::WayfireImpl : public QThread
{
    Q_OBJECT

  public:
    WayfireImpl(QObject *parent = nullptr);
    ~WayfireImpl();

    /** Stop polling This will terminate the thread and delete this object
     */
    void stop();

    /** Request the compositor something */
    uint32_t request(QJsonDocument req);

    /** One FD for reading, one for writing */
    struct pollfd wfSock;

    /** The socket address */
    QString wfSockPath;

    /** Stop the loop flag */
    volatile bool mTerminate = false;

    /** Flag to check if we're connected or not */
    volatile bool mConnected = false;

    /** Function to write the json to wayfire socket. */
    bool writeJson(QJsonDocument j);

    /** Function to read the data from wayfire socket and parse it into json */
    QJsonDocument readJson();

  private:
    QList<uint> mPendingRequests;
    QMutex mutex; // Add a mutex for thread safety

    /** Function to read the json from wayfire socket into a buffer. */
    bool readExact(char *buf, uint size);

  protected:
    void run() override;

  Q_SIGNALS:
    /** We recieved an event from Wayfire */
    void wayfireEvent(QJsonDocument);

    /** Relay the message received from the server */
    void response(uint32_t, QJsonDocument);

    /** Inform the rest that polling has started */
    void started();
};


class LXQt::Panel::Wayfire : public QObject
{
    Q_OBJECT

  public:
    Wayfire(const QString &wfSock = QString());

    /** Connect to Wayfire */
    bool connectToServer() const;

    /* ========== Specific requests ========== */

    /** Request the compositor to send us the outputs information */
    QJsonArray listOutputs() const;

    /** Request the compositor to send us the outputs information */
    WaylandId getActiveOutput() const;

    /** Request the compositor to send us information of a given output */
    QJsonObject getOutputInfo(WaylandId opId) const;

    /** Request the compositor to focus a given output */
    bool focusOutput(WaylandId opId) const;

    /** Request the compositor to trigger show-desktop a given output */
    bool showDesktop(WaylandId opId) const;

    /** Request the compositor to send us the wsets information */
    QJsonArray getWorkspaceSetsInfo() const;

    /** Request the compositor to send us the wsets information */
    QString getWorkspaceName(int, const QString &outputName = QString()) const;
    bool setWorkspaceName(int, const QString&) const;

    /** Request the compositor to change the current workspace */
    bool switchToWorkspace(WaylandId opId, int64_t nth, WaylandId viewId = WaylandId()) const;

    /** Request the compositor to send us the list of views */
    QJsonArray listViews() const;

    /** Request the compositor to send us the info of the given view */
    WaylandId getActiveView() const;

    /** Request the compositor to send us the info of the given view */
    QJsonObject getViewInfo(WaylandId viewId) const;

    /** Request the compositor to focus a given view */
    bool focusView(WaylandId viewId) const;

    /** Request the compositor to minimize/restore a given view */
    bool minimizeView(WaylandId viewId, bool) const;

    /** Request the compositor to maximize/restore a given view */
    bool maximizeView(WaylandId viewId, int edges) const;

    /** Request the compositor to fullscreen a given view */
    bool fullscreenView(WaylandId viewId, bool) const;

    /** Request the compositor to focus a given view */
    bool restoreView(WaylandId viewId) const;

    /** Request the compositor to focus a given view */
    bool sendViewToWorkspace(WaylandId viewId, int nth) const;

    /** Request the compositor to focus a given view */
    bool closeView(WaylandId viewId) const;

    /** This is a generic request */
    QJsonDocument genericRequest(QJsonDocument) const;

    /* ========== WAYFIRE EVENTS ========== */

    /**
     * A new output was added.
     */
    Q_SIGNAL void outputAdded(QJsonDocument);

    /**
     * An existing output was removed.
     */
    Q_SIGNAL void outputRemoved(QJsonDocument);

    /**
     * A particular output has gained focus. Only one output can have focus at any given time. This outupt can
     * be used to calculate the focused view.
     */
    Q_SIGNAL void outputFocused(QJsonDocument);

    /**
     * Current workspace set of a given output was changed When the wset of an output changes, always query
     * the workspace
     */
    Q_SIGNAL void workspaceSetChanged(QJsonDocument);

    /**
     * The active workspace of a given wset changed. Because wsets of different outputs are independent,
     * workspace change on one output will not affect the other.
     */
    Q_SIGNAL void workspaceChanged(QJsonDocument);

    /**
     * A view was just mapped. This view does not have an output nor a wset. Even the title and app-id will be
     * unset. Only the view id is valid.
     */
    Q_SIGNAL void viewMapped(QJsonDocument);

    /**
     * A view on a particular outupt gained focus Since wayfire supports independent outputs, each output can
     * have a view with focus. The focused view of the active outupt will be the one with keyboard focus.
     */
    Q_SIGNAL void viewFocused(QJsonDocument);

    /**
     * The title of a view changed. The title can be "nil" or null. In such cases, the user can safely ignore
     * it.
     */
    Q_SIGNAL void viewTitleChanged(QJsonDocument);

    /**
     * The app-id of a view changed. The app-id can be "nil" or null. In such cases, the user can safely
     * ignore it.
     */
    Q_SIGNAL void viewAppIdChanged(QJsonDocument);

    /**
     * The geometry of a view changed.
     */
    Q_SIGNAL void viewGeometryChanged(QJsonDocument);

    /**
     * The tiled status of a view has changed.
     */
    Q_SIGNAL void viewTiled(QJsonDocument);

    /**
     * A view was minimized.
     */
    Q_SIGNAL void viewMinimized(QJsonDocument);

    /**
     * The output of a view changed. When the view gets mapped, the output will be null.
     */
    Q_SIGNAL void viewOutputChanged(QJsonDocument);

    /**
     * The workspace of a view changed.
     */
    Q_SIGNAL void viewWorkspaceChanged(QJsonDocument);

    /**
     * An existing view was unmapped. The output, wset etc of this view are invalid as of now.
     */
    Q_SIGNAL void viewUnmapped(QJsonDocument);

    /**
     * A generic wayfire event. This signal is always emitted for all events. Specific signals are emitted by
     * parsing this event.
     */
    Q_SIGNAL void genericEvent(QJsonDocument);

    /**
     * Inform the user tha there was an error. Currently used to indicate failure in watching for wayfire
     * events.
     */
    Q_SIGNAL void error();

  private:
    /** Implementation class pointer */
    QPointer<WayfireImpl> impl;

    /** Partially parse the events to emit the correct signals */
    void parseEvents(QJsonDocument);
};
