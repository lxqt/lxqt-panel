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

#include "wayfire-common.h"

// Socket related
#include <sys/socket.h>
#include <sys/un.h>

// Other headers
#include <unistd.h>
#include <errno.h>


LXQt::Panel::WayfireImpl::WayfireImpl() : QRunnable()
{
    wfSock.fd = -1;
}

void LXQt::Panel::WayfireImpl::stop()
{
    mTerminate = true;

    if (wfSock.fd != -1)
    {
        close(wfSock.fd);
        wfSock.fd = -1;
    }

    qApp->processEvents();
}

uint32_t LXQt::Panel::WayfireImpl::request(QJsonDocument req)
{
    QMutexLocker locker(&mutex);

    if (!mConnected)
    {
        return 0;
    }

    writeJson(req);

    // Generate a unique request ID
    static std::atomic<uint32_t> requestCounter(0);
    uint32_t reqId = ++requestCounter;

    if (requestCounter == UINT32_MAX)
    {
        requestCounter = 1;
    }

    mPendingRequests << reqId;
    return reqId;
}

void LXQt::Panel::WayfireImpl::run()
{
    emit started();

    while (true)
    {
        int nready = poll(&wfSock, 1, 10);

        /** Something went wrong while polling */
        if (nready < 0)
        {
            qWarning() << "[Error]:" << strerror(errno);
        }

        /** Bye bye..! */
        if (mTerminate)
        {
            return;
        }

        /** Nothing to read */
        if (nready == 0)
        {
            continue;
        }

        /** We have something to read. Let's see what it is. */
        if (wfSock.revents & (POLLRDNORM | POLLERR))
        {
            QJsonDocument resp = readJson();

            /** This is an event */
            if (resp.isObject() && resp.object().contains(QStringLiteral("event")))
            {
                emit wayfireEvent(resp);
            }
            /** This is the response to a request */
            else
            {
                // Lock the mutex
                QMutexLocker locker(&mutex);

                uint32_t reqId = mPendingRequests.takeFirst();

                // Emit signal asynchronously
                QMetaObject::invokeMethod(this, [this, reqId, resp] ()
                {
                    emit response(reqId, resp);
                }, Qt::QueuedConnection);

                emit response(reqId, resp);
            }
        }
    }
}

bool LXQt::Panel::WayfireImpl::writeJson(QJsonDocument j)
{
    QByteArray str = j.toJson(QJsonDocument::Compact);
    uint32_t size  = str.size();

    // Write the size of the JSON data
    ssize_t ret = write(wfSock.fd, &size, sizeof(size));

    if (ret == -1)
    {
        // Handle write error
        qDebug() << "Failed to write size to socket:" << strerror(errno);
        // throw std::runtime_error("Failed to write size to socket");
    } else if (ret != sizeof(size))
    {
        // Handle partial write
        qDebug() << "Partial write of size to socket:" << ret << "bytes written, expected" << sizeof(size);
        // throw std::runtime_error("Partial write of size to socket");
    }

    // Write the JSON data
    const char *data     = str.constData();
    ssize_t bytesWritten = 0;

    while (bytesWritten < str.size())
    {
        ret = write(wfSock.fd, data + bytesWritten, str.size() - bytesWritten);

        if (ret == -1)
        {
            // Handle write error
            qDebug() << "Failed to write JSON data to socket:" << strerror(errno);
            return false;
        } else if (ret == 0)
        {
            // Handle socket closed by peer
            qDebug() << "Socket closed by peer while writing JSON data";
            return false;
        }

        bytesWritten += ret;
    }

    /** We succeeded in writing the JSON data successfully */
    return true;
}

QJsonDocument LXQt::Panel::WayfireImpl::readJson()
{
    uint32_t msgSize;

    if (!readExact(reinterpret_cast<char*>(&msgSize), sizeof(msgSize)))
    {
        return QJsonDocument();
    }

    QByteArray buffer(msgSize, Qt::Uninitialized);
    if (!readExact(buffer.data(), msgSize))
    {
        return QJsonDocument();
    }

    return QJsonDocument::fromJson(buffer);
}

bool LXQt::Panel::WayfireImpl::readExact(char *buf, uint size)
{
    while (size > 0)
    {
        int ret = read(wfSock.fd, buf, size);

        if (ret == -1)
        {
            qCritical() << "Failed to read from socket: " << strerror(errno);
            return false;
        }

        buf  += ret;
        size -= ret;
    }

    return true;
}

// Helper function to create a QJsonDocument from an initializer list
QJsonDocument createJsonObject(std::initializer_list<std::pair<QString, QJsonValue>> initList)
{
    QJsonObject obj;

    for (const auto& pair : initList)
    {
        obj.insert(pair.first, pair.second);
    }

    return QJsonDocument(obj);
}

LXQt::Panel::Wayfire::Wayfire(QString wfSock) : QObject()
{
    impl.reset(new WayfireImpl());
    impl->wfSockPath = (wfSock.isEmpty() ? qEnvironmentVariable("WAYFIRE_SOCKET") : wfSock);

    /** Always emit this for any wayfire event */
    connect(impl.data(), &LXQt::Panel::WayfireImpl::wayfireEvent, this, &LXQt::Panel::Wayfire::genericEvent);

    /** Parse the events and emit the correct signal */
    connect(impl.data(), &LXQt::Panel::WayfireImpl::wayfireEvent, this, &LXQt::Panel::Wayfire::parseEvents);
}

LXQt::Panel::Wayfire::~Wayfire()
{
    impl->stop();

    if (impl->wfSock.fd != -1)
    {
        close(impl->wfSock.fd);
        impl->wfSock.fd = -1;
    }

    impl.reset(nullptr);
}

bool LXQt::Panel::Wayfire::connectToServer() const
{
    impl->wfSock.fd     = socket(AF_UNIX, SOCK_STREAM, 0);
    impl->wfSock.events = POLLRDNORM;

    if (impl->wfSock.fd == -1)
    {
        qCritical() << "Failed to create socket: " << strerror(errno);
        return false;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX,

    strncpy(addr.sun_path, impl->wfSockPath.toUtf8().data(), sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (::connect(impl->wfSock.fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        qCritical() << "Failed to connect to socket: " << strerror(errno);
        qCritical() << "Ensure that ipc and ipc-rules plugins are enabled.";
        qCritical() <<
            "If these plugins were enabled after starting this session, please restart the session.";
        close(impl->wfSock.fd);
        return false;
    }

    impl->mConnected = true;

    /** Run the impl in a separate thread */
    QThreadPool::globalInstance()->start(impl.data());

    /** Make a request and forget about it. */
    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("window-rules/events/watch");
    genericRequest(QJsonDocument(request));

    return true;
}

QJsonArray LXQt::Panel::Wayfire::listOutputs() const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("window-rules/list-outputs");

    QJsonDocument response = genericRequest(QJsonDocument(request));

    if (response.isArray())
    {
        return response.array();
    }

    return QJsonArray();
}

WaylandId LXQt::Panel::Wayfire::getActiveOutput() const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("window-rules/get-focused-output");

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"))
    {
        QJsonObject opInfo = reply[QStringLiteral("info")].toObject();
        uint32_t opId = opInfo[QStringLiteral("id")].toInt();

        return WaylandId(opId);
    }

    return WaylandId();
}

bool LXQt::Panel::Wayfire::focusOutput(WaylandId opId) const
{
    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("oswitch/switch-output");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("output-id"), QJsonValue::fromVariant((quint64)opId)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    if (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"))
    {
        return true;
    }

    return false;
}

bool LXQt::Panel::Wayfire::showDesktop(WaylandId opId) const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("wm-actions/toggle_showdesktop");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("output_id"), QJsonValue::fromVariant((quint64)opId)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

QJsonObject LXQt::Panel::Wayfire::getWorkspaceSetsInfo() const
{
    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("window-rules/list-wsets");

    return genericRequest(QJsonDocument(request)).object();
}

QJsonObject LXQt::Panel::Wayfire::getWorkspaceName(int x) const
{
    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("wayfire/get-config-option");
    request[QStringLiteral("option")] = QStringLiteral("workspace-names/eDP-1_workspace_1");

    QJsonDocument wsNameDoc = genericRequest(QJsonDocument(request));

    qDebug() << wsNameDoc.toJson().constData();

    return wsNameDoc.object();
}

bool LXQt::Panel::Wayfire::setWorkspaceName(int, QString) const
{
    return false;
}

bool LXQt::Panel::Wayfire::switchToWorkspace(WaylandId opId, int64_t nth) const
{
    int64_t row;
    int64_t col;

    QJsonObject wsetsInfo = getWorkspaceSetsInfo();
    int64_t nRows = wsetsInfo[ QStringLiteral("grid_height")].toInt();
    int64_t nCols = wsetsInfo[ QStringLiteral("grid_width")].toInt();

    row = (int)(nth / nRows);
    col = nth % nCols;

    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("vswitch/set-workspace");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("output-id"), QJsonValue::fromVariant((quint64)opId)},
        {QStringLiteral("x"), QJsonValue::fromVariant((quint64)col)},
        {QStringLiteral("y"), QJsonValue::fromVariant((quint64)row)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

QJsonArray LXQt::Panel::Wayfire::listViews() const
{
    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("window-rules/list-views");

    return genericRequest(QJsonDocument(request)).array();
}

WaylandId LXQt::Panel::Wayfire::getActiveView() const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("window-rules/get-focused-view");

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"))
    {
        QJsonObject viewInfo = reply[QStringLiteral("info")].toObject();
        uint32_t viewId = viewInfo[QStringLiteral("id")].toInt();

        return WaylandId(viewId);
    }

    return WaylandId();
}

QJsonObject LXQt::Panel::Wayfire::getViewInfo(WaylandId viewId) const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("window-rules/view-info");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"))
    {
        return reply[QStringLiteral("info")].toObject();
    }

    return QJsonObject();
}

bool LXQt::Panel::Wayfire::focusView(WaylandId viewId) const
{
    QJsonObject viewInfo = getViewInfo(viewId);

    if ((viewInfo.isEmpty() == false) && (viewInfo[QStringLiteral("minimized")].toBool() == true))
    {
        minimizeView(viewId, false);
    }

    QJsonObject request;
    request[QStringLiteral("method")] = QStringLiteral("window-rules/focus-view");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

bool LXQt::Panel::Wayfire::minimizeView(WaylandId viewId, bool yes) const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("wm-actions/set-minimized");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("view_id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    request[QStringLiteral("data")] = QJsonObject({
        {QStringLiteral("state"), yes}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    if (reply[QStringLiteral("result")] != QStringLiteral("ok"))
    {
        qDebug() << QJsonDocument(reply).toJson().data() << "\n";
    }

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

bool LXQt::Panel::Wayfire::maximizeView(WaylandId viewId, int edges) const
{
    QJsonObject viewInfo = getViewInfo( viewId );

}

bool LXQt::Panel::Wayfire::fullscreenView(WaylandId viewId, bool yes) const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("wm-actions/set-fullscreen");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("view_id"), QJsonValue::fromVariant((quint64)viewId)},
        {QStringLiteral("state"), yes}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

bool LXQt::Panel::Wayfire::restoreView(WaylandId viewId) const
{
    QJsonObject viewInfo = getViewInfo(viewId);

    if (viewInfo.isEmpty())
    {
        return false;
    }

    /** If it's minimized, unminimize it */
    if (viewInfo[QStringLiteral("minimized")] == true)
    {
        return minimizeView(viewId, false);
    }
    /** The view is maximized, unmaximize it */
    else if (viewInfo[QStringLiteral("fullscreen")] == true)
    {
        return fullscreenView(viewId, false);
    }
    /** The view is maximized, unmaximize it */
    else if (viewInfo[QStringLiteral("tiled-edges")] == 15)
    {
        return maximizeView(viewId, false);
    }

    return false;
}

bool LXQt::Panel::Wayfire::closeView(WaylandId viewId) const
{
    QJsonObject request;

    request[QStringLiteral("method")] = QStringLiteral("window-rules/close-view");
    request[QStringLiteral("data")]   = QJsonObject({
        {QStringLiteral("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QStringLiteral("result")].toString() == QStringLiteral("ok"));
}

QJsonDocument LXQt::Panel::Wayfire::genericRequest(QJsonDocument request) const
{
    if (!impl->mConnected)
    {
        QJsonDocument reply{
            {QStringLiteral("result"), QStringLiteral("failed")}
        };
        return reply;
    }

    uint32_t reqId = impl->request(request);
    std::shared_ptr<QJsonDocument> reply = std::make_shared<QJsonDocument>();

    QEventLoop loop;

    auto connection = connect(
        impl.data(), &LXQt::Panel::WayfireImpl::response, &loop,
        [&reply, reqId, &loop] (uint32_t id, QJsonDocument response)
    {
        if (id == reqId)
        {
            *reply = response;  // Update the content of the shared_ptr
            loop.quit();
        }
    });

    loop.exec();

    // Disconnect the signal-slot connection to avoid any potential issues
    disconnect(connection);

    // Return the QJsonDocument, not the shared_ptr
    return *reply;
}

void LXQt::Panel::Wayfire::parseEvents(QJsonDocument response)
{
    QString event = response[QStringLiteral("event")].toString();

    if (event == QStringLiteral("view-mapped"))
    {
        emit viewMapped(response);
    } else if (event == QStringLiteral("view-focused"))
    {
        emit viewFocused(response);
    } else if (event == QStringLiteral("view-title-changed"))
    {
        emit viewTitleChanged(response);
    } else if (event == QStringLiteral("view-app-id-changed"))
    {
        emit viewAppIdChanged(response);
    } else if (event == QStringLiteral("view-geometry-changed"))
    {
        emit viewGeometryChanged(response);
    } else if (event == QStringLiteral("view-tiled"))
    {
        emit viewTitleChanged(response);
    } else if (event == QStringLiteral("view-minimized"))
    {
        emit viewMinimized(response);
    } else if (event == QStringLiteral("view-set-output"))
    {
        emit viewOutputChanged(response);
    } else if (event == QStringLiteral("view-workspace-changed"))
    {
        emit viewWorkspaceChanged(response);
    } else if (event == QStringLiteral("view-unmapped"))
    {
        emit viewUnmapped(response);
    } else if (event == QStringLiteral("output-added"))
    {
        emit outputAdded(response);
    } else if (event == QStringLiteral("output-removed"))
    {
        emit outputRemoved(response);
    } else if (event == QStringLiteral("output-gain-focus"))
    {
        emit outputFocused(response);
    } else if (event == QStringLiteral("output-wset-changed"))
    {
        emit workspaceSetChanged(response);
    } else if (event == QStringLiteral("wset-workspace-changed"))
    {
        emit workspaceChanged(response);
    } else
    {
        emit genericEvent(response);
    }
}
