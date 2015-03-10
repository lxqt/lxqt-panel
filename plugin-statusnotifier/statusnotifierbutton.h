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

#include <dbusmenuimporter.h>

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
    StatusNotifierButton(QString service, QString objectPath);

public slots:
    void showMenu();

    void newIcon();
    void newToolTip();

private:
    QMenu *menu;
    org::kde::StatusNotifierItem *interface;

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

signals:
    void midButtonPressed(int x, int y);
    void wheelRotated(int delta, QString orientation);
};

#endif // STATUSNOTIFIERBUTTON_H
