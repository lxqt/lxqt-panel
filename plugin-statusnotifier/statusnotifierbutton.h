#ifndef STATUSNOTIFIERBUTTON_H
#define STATUSNOTIFIERBUTTON_H

#include "../panel/ilxqtpanelplugin.h"

#include <QDBusArgument>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDir>
#include <QMenu>
#include <QMouseEvent>
#include <QToolButton>
#include <QWheelEvent>

#include "statusnotifieriteminterface.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
template <typename T> inline T qFromUnaligned(const uchar *src)
{
    T dest;
    const size_t size = sizeof(T);
    memcpy(&dest, src, size);
    return dest;
}
#endif

class StatusNotifierButton : public QToolButton
{
    Q_OBJECT

public:
    StatusNotifierButton(QString service, QString objectPath, QWidget *parent = 0);

    enum Status
    {
        Passive, Active, NeedsAttention
    };

public slots:
    void newIcon();
    void newAttentionIcon();
    void newOverlayIcon();
    void newToolTip();
    void newStatus(QString status);

private:
    org::kde::StatusNotifierItem *interface;
    Status mStatus;

    QIcon icon, overlayIcon, attentionIcon;

protected:
    void contextMenuEvent(QContextMenuEvent * event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void refetchIcon(Status status);
    void resetIcon();
};

#endif // STATUSNOTIFIERBUTTON_H
