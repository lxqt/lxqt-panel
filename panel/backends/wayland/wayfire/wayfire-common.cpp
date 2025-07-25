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


#define QSL QStringLiteral

LXQt::Panel::WayfireImpl::WayfireImpl(QObject *parent) : QThread(parent)
{
    wfSock.fd = -1;
}

LXQt::Panel::WayfireImpl::~WayfireImpl()
{
    stop();
    wait(1000);
}

void LXQt::Panel::WayfireImpl::stop()
{
    mTerminate = true;

    if (wfSock.fd != -1)
    {
        close(wfSock.fd);
        wfSock.fd = -1;
    }
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
            if (resp.isObject() && resp.object().contains(QSL("event")))
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
        qWarning() << "Failed to write size to socket:" << strerror(errno);
        // throw std::runtime_error("Failed to write size to socket");
    } else if (ret != sizeof(size))
    {
        // Handle partial write
        qWarning() << "Partial write of size to socket:" << ret << "bytes written, expected" << sizeof(size);
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
            qWarning() << "Failed to write JSON data to socket:" << strerror(errno);
            return false;
        } else if (ret == 0)
        {
            // Handle socket closed by peer
            qWarning() << "Socket closed by peer while writing JSON data";
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

LXQt::Panel::Wayfire::Wayfire(const QString &wfSock) : QObject()
{
    impl = new WayfireImpl(this);
    connect(impl, &QThread::finished, impl, &QObject::deleteLater);
    impl->wfSockPath = (wfSock.isEmpty() ? qEnvironmentVariable("WAYFIRE_SOCKET") : wfSock);

    /** Always emit this for any wayfire event */
    connect(impl, &LXQt::Panel::WayfireImpl::wayfireEvent, this, &LXQt::Panel::Wayfire::genericEvent);

    /** Parse the events and emit the correct signal */
    connect(impl, &LXQt::Panel::WayfireImpl::wayfireEvent, this, &LXQt::Panel::Wayfire::parseEvents);
}

bool LXQt::Panel::Wayfire::connectToServer() const
{
    if (!impl)
        return false;

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
    impl->start();

    /** Make a request and forget about it. */
    QJsonObject request;
    request[QSL("method")] = QSL("window-rules/events/watch");
    genericRequest(QJsonDocument(request));

    return true;
}

QJsonArray LXQt::Panel::Wayfire::listOutputs() const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/list-outputs");

    QJsonDocument response = genericRequest(QJsonDocument(request));

    if (response.isArray())
    {
        return response.array();
    }

    return QJsonArray();
}

QJsonObject LXQt::Panel::Wayfire::getOutputInfo(WaylandId opId) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/output-info");
    request[QSL("data")]   = QJsonObject({
        {QSL("id"), QJsonValue::fromVariant((quint64)opId)},
    });

    return genericRequest(QJsonDocument(request)).object();
}

WaylandId LXQt::Panel::Wayfire::getActiveOutput() const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/get-focused-output");

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QSL("result")].toString() == QSL("ok"))
    {
        QJsonObject opInfo = reply[QSL("info")].toObject();
        uint32_t opId = opInfo[QSL("id")].toInt();

        return WaylandId(opId);
    }

    return WaylandId();
}

bool LXQt::Panel::Wayfire::focusOutput(WaylandId opId) const
{
    QJsonObject request;
    request[QSL("method")] = QSL("oswitch/switch-output");
    request[QSL("data")]   = QJsonObject({
        {QSL("output-id"), QJsonValue::fromVariant((quint64)opId)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    if (reply[QSL("result")].toString() == QSL("ok"))
    {
        return true;
    }

    return false;
}

bool LXQt::Panel::Wayfire::showDesktop(WaylandId opId) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("wm-actions/toggle_showdesktop");
    request[QSL("data")]   = QJsonObject({
        {QSL("output_id"), QJsonValue::fromVariant((quint64)opId)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QSL("result")].toString() == QSL("ok"));
}

QJsonArray LXQt::Panel::Wayfire::getWorkspaceSetsInfo() const
{
    QJsonObject request;
    request[QSL("method")] = QSL("window-rules/list-wsets");

    return genericRequest(QJsonDocument(request)).array();
}

QString LXQt::Panel::Wayfire::getWorkspaceName(int x, const QString &outputName) const
{
    QString targetKey = QString::fromUtf8("%1_workspace_%2").arg(outputName).arg(x);

    QJsonObject request;
    request[QSL("method")] = QSL("wayfire/get-config-option");
    request[QSL("data")]   = QJsonObject({
        {QSL("option"), QSL("workspace-names/names")},
    });

    QJsonObject wsNamesObj = genericRequest(QJsonDocument(request)).object();

    QJsonArray wsNameList = wsNamesObj[QSL("value")].toArray();

    for ( int i = 0; i < wsNameList.size(); i++ )
    {
        QJsonArray wsNamePair = wsNameList[i].toArray();

        if (wsNamePair.size() != 2)
        {
            continue;
        }

        QString key = wsNamePair[0].toString();
        if (key == targetKey)
        {
            QString name = wsNamePair[1].toString();
            return name;
        }
    }

    return QString(); // the front-end sets the name
}

bool LXQt::Panel::Wayfire::setWorkspaceName(int, const QString&) const
{
    return false;
}

bool LXQt::Panel::Wayfire::switchToWorkspace(WaylandId opId, int64_t nth, WaylandId viewId) const
{
    QJsonObject wsetsInfo = getWorkspaceSetsInfo().at(0).toObject();
    QJsonObject workspace = wsetsInfo[QSL("workspace")].toObject();
    int64_t nCols = workspace[QSL("grid_width")].toInt();

    int64_t row = floor((nth - 1) / nCols);
    int64_t col = (nth - 1) % nCols;

    QJsonObject request;
    request[QSL("method")] = QSL("vswitch/set-workspace");
    if (viewId)
    {
        request[QSL("data")] = QJsonObject({
            {QSL("output-id"), QJsonValue::fromVariant((quint64)opId)},
            {QSL("x"), QJsonValue::fromVariant((quint64)col)},
            {QSL("y"), QJsonValue::fromVariant((quint64)row)},
            {QSL("view-id"), QJsonValue::fromVariant((quint64)viewId)},
        });
    } else
    {
        request[QSL("data")] = QJsonObject({
            {QSL("output-id"), QJsonValue::fromVariant((quint64)opId)},
            {QSL("x"), QJsonValue::fromVariant((quint64)col)},

            {QSL("y"), QJsonValue::fromVariant((quint64)row)},
        });
    }

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QSL("result")].toString() == QSL("ok"));
}

QJsonArray LXQt::Panel::Wayfire::listViews() const
{
    QJsonObject request;
    request[QSL("method")] = QSL("window-rules/list-views");

    QJsonArray response = genericRequest(QJsonDocument(request)).array();

    QJsonArray views;
    for ( QJsonValue viewVal : response )
    {
        if (viewVal.isObject())
        {
            QJsonObject view = viewVal.toObject();
            // Ghost windows of Xwayland
            if (view[QSL("pid")].toInt() == -1)
            {
                continue;
            }

            // Proper toplevel and mapped views
            if (view[QSL("role")].toString() == QSL("toplevel") && view[QSL("mapped")].toBool())
            {
                views << view;
            }
        }
    }

    return views;
}

WaylandId LXQt::Panel::Wayfire::getActiveView() const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/get-focused-view");

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QSL("result")].toString() == QSL("ok"))
    {
        QJsonObject viewInfo = reply[QSL("info")].toObject();
        uint32_t viewId = viewInfo[QSL("id")].toInt();

        return WaylandId(viewId);
    }

    return WaylandId();
}

QJsonObject LXQt::Panel::Wayfire::getViewInfo(WaylandId viewId) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/view-info");
    request[QSL("data")]   = QJsonObject({
        {QSL("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    if (reply[QSL("result")].toString() == QSL("ok"))
    {
        return reply[QSL("info")].toObject();
    }

    return QJsonObject();
}

bool LXQt::Panel::Wayfire::focusView(WaylandId viewId) const
{
    QJsonObject viewInfo = getViewInfo(viewId);

    if ((viewInfo.isEmpty() == false) && (viewInfo[QSL("minimized")].toBool() == true))
    {
        minimizeView(viewId, false);
    }

    QJsonObject request;
    request[QSL("method")] = QSL("window-rules/focus-view");
    request[QSL("data")]   = QJsonObject({
        {QSL("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonObject reply = genericRequest(QJsonDocument(request)).object();

    return (reply[QSL("result")].toString() == QSL("ok"));
}

bool LXQt::Panel::Wayfire::minimizeView(WaylandId viewId, bool yes) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("wm-actions/set-minimized");
    request[QSL("data")]   = QJsonObject({
        {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
        {QSL("state"), yes}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    if (reply[QSL("result")] != QSL("ok"))
    {
        qWarning() << QJsonDocument(reply).toJson().data() << "\n";
    }

    return (reply[QSL("result")].toString() == QSL("ok"));
}

bool LXQt::Panel::Wayfire::maximizeView(WaylandId viewId, int edges) const
{
    QJsonObject request;

    /**
     * Support for this does not yet exist in Wayfire. Pending PR from Marcus Britanicus.
     */

    // request[QSL("method")] = QSL("wm-actions/set-tiled");
    // request[QSL("data")]   = QJsonObject({
    //     {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
    //     {QSL("view-id"), QJsonValue::fromVariant((quint64)viewId)},
    //     {QSL("id"), QJsonValue::fromVariant((quint64)viewId)},
    //     {QSL("edges"), edges}
    // });

    request[QSL("method")] = (edges ? QSL("grid/slot_c") : QSL("grid/restore"));
    request[QSL("data")]   = QJsonObject({
        {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    if (reply[QSL("result")] != QSL("ok"))
    {
        qWarning() << QJsonDocument(reply).toJson().data() << "\n";
    }

    return (reply[QSL("result")].toString() == QSL("ok"));
}

bool LXQt::Panel::Wayfire::fullscreenView(WaylandId viewId, bool yes) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("wm-actions/set-fullscreen");
    request[QSL("data")]   = QJsonObject({
        {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
        {QSL("state"), yes}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QSL("result")].toString() == QSL("ok"));
}

bool LXQt::Panel::Wayfire::restoreView(WaylandId viewId) const
{
    QJsonObject viewInfo = getViewInfo(viewId);

    if (viewInfo.isEmpty())
    {
        return false;
    }

    /** If it's minimized, unminimize it */
    if (viewInfo[QSL("minimized")] == true)
    {
        return minimizeView(viewId, false);
    }
    /** The view is fullscreened, un-fullscreen it */
    else if (viewInfo[QSL("fullscreen")] == true)
    {
        return fullscreenView(viewId, false);
    }
    /** The view is maximized, unmaximize it */
    else if (viewInfo[QSL("tiled-edges")] != 0)
    {
        return maximizeView(viewId, false);
    }

    return false;
}

bool LXQt::Panel::Wayfire::sendViewToWorkspace(WaylandId viewId, int nth) const
{
    QJsonObject request;

    /** Set view sticky */
    if ( nth == 0 ) {
        QJsonObject viewInfo = getViewInfo( viewId );
        request[QSL("method")] = QSL("wm-actions/set-sticky");
        request[QSL("data")]   = QJsonObject({
            {QSL("view-id"), QJsonValue::fromVariant((quint64)viewId)},
            {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
            {QSL("state"), !viewInfo[QSL("sticky")].toBool()},
        });
    }

    /** Send view to a specific desktop */
    else {
        QJsonObject wsetsInfo = getWorkspaceSetsInfo().at(0).toObject();
        QJsonObject workspace = wsetsInfo[QSL("workspace")].toObject();
        QJsonObject viewInfo  = getViewInfo(viewId);

        int64_t nCols = workspace[QSL("grid_width")].toInt();

        int64_t row = floor((nth - 1) / nCols);
        int64_t col = (nth - 1) % nCols;

        quint64 opId = viewInfo[QSL("output-id")].toInt();

        request[QSL("method")] = QSL("vswitch/send-view");
        request[QSL("data")]   = QJsonObject({
            {QSL("output-id"), QJsonValue::fromVariant((quint64)opId)},
            {QSL("x"), QJsonValue::fromVariant((quint64)col)},
            {QSL("y"), QJsonValue::fromVariant((quint64)row)},
            {QSL("view-id"), QJsonValue::fromVariant((quint64)viewId)},
            {QSL("view_id"), QJsonValue::fromVariant((quint64)viewId)},
        });
    }

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QSL("result")].toString() == QSL("ok"));
}

bool LXQt::Panel::Wayfire::closeView(WaylandId viewId) const
{
    QJsonObject request;

    request[QSL("method")] = QSL("window-rules/close-view");
    request[QSL("data")]   = QJsonObject({
        {QSL("id"), QJsonValue::fromVariant((quint64)viewId)}
    });

    QJsonDocument reply = genericRequest(QJsonDocument(request));

    return (reply[QSL("result")].toString() == QSL("ok"));
}

QJsonDocument LXQt::Panel::Wayfire::genericRequest(QJsonDocument request) const
{
    if (!impl || !impl->mConnected)
    {
        QJsonDocument reply{
            {QSL("result"), QSL("failed")}
        };
        return reply;
    }

    uint32_t reqId = impl->request(request);
    std::shared_ptr<QJsonDocument> reply = std::make_shared<QJsonDocument>();

    QEventLoop loop;

    auto connection = connect(
        impl, &LXQt::Panel::WayfireImpl::response, &loop,
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
    QString event = response[QSL("event")].toString();

    if (event == QSL("view-mapped"))
    {
        emit viewMapped(response);
    } else if (event == QSL("view-focused"))
    {
        emit viewFocused(response);
    } else if (event == QSL("view-title-changed"))
    {
        emit viewTitleChanged(response);
    } else if (event == QSL("view-app-id-changed"))
    {
        emit viewAppIdChanged(response);
    } else if (event == QSL("view-geometry-changed"))
    {
        emit viewGeometryChanged(response);
    } else if (event == QSL("view-tiled"))
    {
        emit viewTiled(response);
    } else if (event == QSL("view-minimized"))
    {
        emit viewMinimized(response);
    } else if (event == QSL("view-set-output"))
    {
        emit viewOutputChanged(response);
    } else if (event == QSL("view-workspace-changed"))
    {
        emit viewWorkspaceChanged(response);
    } else if (event == QSL("view-unmapped"))
    {
        emit viewUnmapped(response);
    } else if (event == QSL("output-added"))
    {
        emit outputAdded(response);
    } else if (event == QSL("output-removed"))
    {
        emit outputRemoved(response);
    } else if (event == QSL("output-gain-focus"))
    {
        emit outputFocused(response);
    } else if (event == QSL("output-wset-changed"))
    {
        emit workspaceSetChanged(response);
    } else if (event == QSL("wset-workspace-changed"))
    {
        emit workspaceChanged(response);
    } else
    {
        emit genericEvent(response);
    }
}
