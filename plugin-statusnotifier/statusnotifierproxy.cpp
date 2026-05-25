/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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

#include "statusnotifierproxy.h"
#include "statusnotifierwatcher.h"

#include <QApplication>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDBusConnectionInterface>
#include <QDebug>

Q_GLOBAL_STATIC(StatusNotifierProxy, statusNotifierProxy)

StatusNotifierProxy::StatusNotifierProxy()
    : mWatcher{nullptr},
    mUsersCount{0}
{
}

void StatusNotifierProxy::createWatcher()
{
    QFutureWatcher<StatusNotifierWatcher *> * future_watcher = new QFutureWatcher<StatusNotifierWatcher *>;
    connect(future_watcher, &QFutureWatcher<StatusNotifierWatcher *>::finished, this, [this, future_watcher]
        {
            mWatcher.reset(future_watcher->future().result());

            connect(mWatcher.get(), &StatusNotifierWatcher::StatusNotifierItemRegistered,
                    this, &StatusNotifierProxy::StatusNotifierItemRegistered);
            connect(mWatcher.get(), &StatusNotifierWatcher::StatusNotifierItemUnregistered,
                    this, &StatusNotifierProxy::StatusNotifierItemUnregistered);

            qDebug() << "StatusNotifierProxy, services:" << mWatcher->RegisteredStatusNotifierItems();

            future_watcher->deleteLater();
        });

    QFuture<StatusNotifierWatcher *> future = QtConcurrent::run([]
        {
            QString dbusName = QStringLiteral("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
            if (QDBusConnectionInterface::ServiceNotRegistered == QDBusConnection::sessionBus().interface()->registerService(dbusName, QDBusConnectionInterface::DontQueueService))
                qDebug() << "unable to register service for " << dbusName;

            StatusNotifierWatcher * watcher = new StatusNotifierWatcher;
            watcher->RegisterStatusNotifierHost(dbusName);
            watcher->moveToThread(QApplication::instance()->thread());
            return watcher;
        });

    future_watcher->setFuture(future);
}

QStringList StatusNotifierProxy::RegisteredStatusNotifierItems() const
{
    Q_ASSERT(mUsersCount > 0);
    return mWatcher ? mWatcher->RegisteredStatusNotifierItems() : QStringList{};
}

StatusNotifierProxy & StatusNotifierProxy::registerLifetimeUsage(QObject * obj)
{
    StatusNotifierProxy & p = *statusNotifierProxy();
    p.registerUsage(obj);
    return p;
}

void StatusNotifierProxy::registerUsage(QObject * obj)
{
    connect(obj, &QObject::destroyed, this, &StatusNotifierProxy::unregisterUsage);
    if (mUsersCount <= 0)
        createWatcher();
    ++mUsersCount;
}

void StatusNotifierProxy::unregisterUsage()
{
    --mUsersCount;
    if (mUsersCount <= 0) {
        mWatcher.reset();
    }
}
