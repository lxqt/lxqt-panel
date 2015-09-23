#if !defined(SNIASYNC_H)
#define SNIASYNC_H

#include <functional>
#include "statusnotifieriteminterface.h"

class SniAsync : public QObject
{
    Q_OBJECT
public:
    SniAsync(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    template <typename T>
    inline void propertyGetAsync(QString const &name, std::function<void (T)> finished)
    {
        connect(new QDBusPendingCallWatcher{asyncPropGet(name), this},
                &QDBusPendingCallWatcher::finished,
                [this, finished, name] (QDBusPendingCallWatcher * call)
                {
                    QDBusPendingReply<QVariant> reply = *call;
                    if (reply.isError())
                        qDebug() << "Error on DBus request:" << reply.error();
                    finished(qdbus_cast<T>(reply.value()));
                    call->deleteLater();
                }
        );
    }

    //exposed methods from org::kde::StatusNotifierItem
    inline QString service() const { return mSni.service(); }

public slots:
    //Forwarded slots from org::kde::StatusNotifierItem
    inline QDBusPendingReply<> Activate(int x, int y) { return mSni.Activate(x, y); }
    inline QDBusPendingReply<> ContextMenu(int x, int y) { return mSni.ContextMenu(x, y); }
    inline QDBusPendingReply<> Scroll(int delta, const QString &orientation) { return mSni.Scroll(delta, orientation); }
    inline QDBusPendingReply<> SecondaryActivate(int x, int y) { return mSni.SecondaryActivate(x, y); }

signals:
    //Forwarded signals from org::kde::StatusNotifierItem
    void NewAttentionIcon();
    void NewIcon();
    void NewOverlayIcon();
    void NewStatus(const QString &status);
    void NewTitle();
    void NewToolTip();

private:
    QDBusPendingReply<QDBusVariant> asyncPropGet(QString const & property);

private:
    org::kde::StatusNotifierItem mSni;

};

#endif
