/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Jes <zjesclean.gmail@gmail.com>
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

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QX11Info>

#include <xcb/xcb.h>
#define explicit _explicit
#include <xcb/xkb.h>

#include "lxqtkbdlayout.h"
#include "lxqtkbdinfo.h"
#include "lxqtkbdkeeper.h"

class LxQtKbdLayoutPrivate: public QAbstractNativeEventFilter
{
    Q_DECLARE_PUBLIC(LxQtKbdLayout)
public:
    LxQtKbdLayoutPrivate(LxQtKbdLayout *qptr, QSettings *set):
        q_ptr(qptr),
        m_settings(set)
    {
        qApp->installNativeEventFilter(this);
    }

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *)
    {
        if (eventType != "xcb_generic_event_t")
            return false;

        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);
        if ((event->response_type & ~0x80) == m_xkbEventBase + XkbEventCode){
            Q_Q(LxQtKbdLayout);
            switch (reinterpret_cast<xcb_xkb_indicator_state_notify_event_t*>(event)->xkbType) {
            case XkbStateNotify:
                m_keeper->updateInfo(reinterpret_cast<xcb_xkb_state_notify_event_t*>(event)->group);
                break;
            case XkbNewKeyboardNotify:
                m_info.readKeyboardInfo();
                emit q->changed(m_info.currentSym());
                break;
            case XkbIndicatorStateNotify:
                indicatorsChanged(reinterpret_cast<xcb_xkb_indicator_state_notify_event_t*>(event));
                break;
            }
        }

        m_keeper->checkLayout();
        return false;
    }

    void indicatorsChanged(xcb_xkb_indicator_state_notify_event_t *event)
    {
        if (event->stateChanged){
            Q_Q(LxQtKbdLayout);
            m_caps = event->state;
            emit q->capsChanged(m_caps);
        }
    }

    bool init()
    {
        int code;
        int major = XkbMajorVersion;
        int minor = XkbMinorVersion;
        int xkbErrorBase;

        Display *disp = QX11Info::display();

        if (!XkbLibraryVersion(&major, &minor)){
            qWarning() << "LxQtKbdLayout: cannot read xkb version";
            return false;
        }

        if (!XkbQueryExtension(disp, &code, &m_xkbEventBase, &xkbErrorBase, &major, &minor)){
            qWarning() << "LxQtKbdLayout: cannot query xkb extension";
            return false;
        }

        if (!XkbUseExtension(disp, &major, &minor)){
            qWarning() << "LxQtKbdLayout: cannot use xkb extension";
            return false;
        }

        if (m_settings->value("showcaps").toBool())
            XkbSelectEvents(disp, XkbUseCoreKbd, XkbIndicatorStateNotifyMask, XkbIndicatorStateNotifyMask);

        XkbSelectEvents(disp, XkbUseCoreKbd, XkbStateNotifyMask, XkbStateNotifyMask);

        m_info.readKeyboardInfo();

        SwitchMode mode = static_cast<SwitchMode>(m_settings->value("switchmode").toInt());
        switch (mode){
        case SwitchMode::Application:
            m_keeper.reset(new LxQtKbdLayoutAppKeeper(m_info));
            qDebug() << "app keeper";
            break;
        case SwitchMode::Window:
            m_keeper.reset(new LxQtKbdLayoutWinKeeper(m_info));
            qDebug() << "win keeper";
            break;
        default:
            m_keeper.reset(new LxQtKbdLayoutKeeper(m_info));
            qDebug() << "no keeper";
            break;
        }

        Q_Q(LxQtKbdLayout);

        q->connect(m_keeper.data(), &LxQtKbdLayoutKeeper::changed, [this, q](){
            emit q->changed(m_info.currentSym());
        });

        XkbGetIndicatorState(disp, XkbUseCoreKbd, &m_caps);

        emit q->capsChanged(m_caps);
        emit q->changed(m_info.currentSym());

        return true;
    }

private:
    LxQtKbdLayout                       *q_ptr;
    QScopedPointer<LxQtKbdLayoutKeeper>  m_keeper;
    QSettings                           *m_settings;
    int                                  m_xkbEventBase;
    LxQtKbdInfo                          m_info;
    unsigned                             m_caps = 0;
};

//--------------------------------------------------------------------------------------------------

LxQtKbdLayout::LxQtKbdLayout(QSettings *set):
    d_ptr(new LxQtKbdLayoutPrivate(this, set))
{}

LxQtKbdLayout::~LxQtKbdLayout()
{}

void LxQtKbdLayout::reset()
{
    Q_D(LxQtKbdLayout);
    d->init();
}
