#include "statusnotifierwatcher.h"
#include <QDebug>

StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<IconPixmap>("IconPixmap");
    qDBusRegisterMetaType<IconPixmap>();
    qRegisterMetaType<IconPixmapList>("IconPixmapList");
    qDBusRegisterMetaType<IconPixmapList>();
    qRegisterMetaType<ToolTip>("ToolTip");
    qDBusRegisterMetaType<ToolTip>();

    serviceWatcher = new QDBusServiceWatcher;
    serviceWatcher->setConnection(QDBusConnection::sessionBus());

    connect(serviceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(removeItem(QString)));
}

StatusNotifierWatcher::~StatusNotifierWatcher()
{

}

void StatusNotifierWatcher::RegisterStatusNotifierItem(QString service)
{
    QString path = "/StatusNotifierItem";

    // workaround for sni-qt
    if (service.contains("/"))
    {
        path = service;
        service = message().service();
    }

    m_services << service;
    serviceWatcher->addWatchedService(service);

    emit StatusNotifierItemRegistered();
    emit itemAdded(service, path);
}

void StatusNotifierWatcher::RegisterStatusNotifierHost(QString service)
{
    Q_UNUSED(service);
}

void StatusNotifierWatcher::removeItem(QString service)
{
    int index = m_services.indexOf(service);
    m_services.removeAt(index);
    serviceWatcher->removeWatchedService(service);

    emit StatusNotifierItemUnregistered();
    emit itemRemoved(index);
}
