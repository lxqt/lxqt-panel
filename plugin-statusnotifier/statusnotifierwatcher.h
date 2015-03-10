#ifndef STATUSNOTIFIERWATCHER_H
#define STATUSNOTIFIERWATCHER_H

#include <QDBusConnection>
#include <QDBusContext>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusServiceWatcher>

#include "dbustypes.h"

class StatusNotifierWatcher : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.StatusNotifierWatcher")
    Q_SCRIPTABLE Q_PROPERTY(bool IsStatusNotifierHostRegistered READ isStatusNotifierHostRegistered)
    Q_SCRIPTABLE Q_PROPERTY(int ProtocolVersion READ protocolVersion)
    Q_SCRIPTABLE Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ registeredStatusNotifierItems)
public:
    explicit StatusNotifierWatcher(QObject *parent = 0);
    ~StatusNotifierWatcher();

    bool isStatusNotifierHostRegistered() const { return true; }
    int protocolVersion() const { return 1; }
    QStringList registeredStatusNotifierItems() const { return m_services; }

    QStringList itemServices() const { return m_services; }
signals:
    Q_SCRIPTABLE void StatusNotifierItemRegistered();
    Q_SCRIPTABLE void StatusNotifierItemUnregistered();
    Q_SCRIPTABLE void StatusNotifierHostRegistered();

    void itemAdded(QString service, QString objectPath);
    void itemRemoved(int index);
public slots:
    Q_SCRIPTABLE void RegisterStatusNotifierItem(QString service);
    Q_SCRIPTABLE void RegisterStatusNotifierHost(QString service);

    void removeItem(QString service);
private:
    QStringList m_services;

    QDBusServiceWatcher *serviceWatcher;
};

#endif // STATUSNOTIFIERWATCHER_H
