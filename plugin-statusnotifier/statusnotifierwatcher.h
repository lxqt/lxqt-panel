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
    Q_SCRIPTABLE Q_PROPERTY(QStringList RegisteredStatusNotifierItems READ RegisteredStatusNotifierItems)

public:
    explicit StatusNotifierWatcher(QObject *parent = 0);
    ~StatusNotifierWatcher();

    bool isStatusNotifierHostRegistered() { return mHosts.count() > 0; }
    int protocolVersion() const { return 0; }
    QStringList RegisteredStatusNotifierItems() const { return mServices; }

signals:
    Q_SCRIPTABLE void StatusNotifierItemRegistered(const QString &service);
    Q_SCRIPTABLE void StatusNotifierItemUnregistered(const QString &service);
    Q_SCRIPTABLE void StatusNotifierHostRegistered();

public slots:
    Q_SCRIPTABLE void RegisterStatusNotifierItem(const QString &serviceOrPath);
    Q_SCRIPTABLE void RegisterStatusNotifierHost(const QString &service);

    void serviceUnregistered(const QString &service);

private:
    QStringList mServices;
    QStringList mHosts;
    QDBusServiceWatcher *mWatcher;
};

#endif // STATUSNOTIFIERWATCHER_H
