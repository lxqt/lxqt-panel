#include "statusnotifierwatcher.h"
#include <QDebug>
#include <QDBusConnectionInterface>

StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<IconPixmap>("IconPixmap");
    qDBusRegisterMetaType<IconPixmap>();
    qRegisterMetaType<IconPixmapList>("IconPixmapList");
    qDBusRegisterMetaType<IconPixmapList>();
    qRegisterMetaType<ToolTip>("ToolTip");
    qDBusRegisterMetaType<ToolTip>();

    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerService("org.kde.StatusNotifierWatcher"))
        qDebug() << QDBusConnection::sessionBus().lastError().message();
    if (!dbus.registerObject("/StatusNotifierWatcher", this, QDBusConnection::ExportScriptableContents))
        qDebug() << QDBusConnection::sessionBus().lastError().message();

    mWatcher = new QDBusServiceWatcher(this);
    mWatcher->setConnection(dbus);
    mWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);

    connect(mWatcher, &QDBusServiceWatcher::serviceUnregistered, this, &StatusNotifierWatcher::serviceUnregistered);
}

StatusNotifierWatcher::~StatusNotifierWatcher()
{
    QDBusConnection::sessionBus().unregisterService("org.kde.StatusNotifierWatcher");
}

void StatusNotifierWatcher::RegisterStatusNotifierItem(const QString &serviceOrPath)
{
    QString service = serviceOrPath;
    QString path = "/StatusNotifierItem";

    // workaround for sni-qt
    if (service.startsWith('/'))
    {
        path = service;
        service = message().service();
    }

    QString notifierItemId = service + path;

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(service).value()
        && !mServices.contains(notifierItemId))
    {
        qDebug() << "Registering" << notifierItemId;

        mServices << notifierItemId;
        mWatcher->addWatchedService(service);
        emit StatusNotifierItemRegistered(notifierItemId);
    }
}

void StatusNotifierWatcher::RegisterStatusNotifierHost(const QString &service)
{
    Q_UNUSED(service);
}

void StatusNotifierWatcher::serviceUnregistered(const QString &service)
{
    qDebug() << "Service" << service << "unregistered";

    mWatcher->removeWatchedService(service);

    QString match = service + '/';
    QStringList::Iterator it = mServices.begin();
    while (it != mServices.end())
    {
        if (it->startsWith(match))
        {
            QString name = *it;
            it = mServices.erase(it);
            emit StatusNotifierItemUnregistered(name);
        }
        else
            ++it;
    }
}
