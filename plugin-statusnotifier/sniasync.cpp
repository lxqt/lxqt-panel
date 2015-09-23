#include "sniasync.h"

SniAsync::SniAsync(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent/* = 0*/)
    : QObject(parent)
    , mSni{service, path, connection}
{
    //forward StatusNotifierItem signals
    connect(&mSni, &org::kde::StatusNotifierItem::NewAttentionIcon, this, &SniAsync::NewAttentionIcon);
    connect(&mSni, &org::kde::StatusNotifierItem::NewIcon, this, &SniAsync::NewIcon);
    connect(&mSni, &org::kde::StatusNotifierItem::NewOverlayIcon, this, &SniAsync::NewOverlayIcon);
    connect(&mSni, &org::kde::StatusNotifierItem::NewStatus, this, &SniAsync::NewStatus);
    connect(&mSni, &org::kde::StatusNotifierItem::NewTitle, this, &SniAsync::NewTitle);
    connect(&mSni, &org::kde::StatusNotifierItem::NewToolTip, this, &SniAsync::NewToolTip);
}


QDBusPendingReply<QDBusVariant> SniAsync::asyncPropGet(QString const & property)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(mSni.service(), mSni.path(), QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    msg << mSni.interface() << property;
    return mSni.connection().asyncCall(msg);
}
