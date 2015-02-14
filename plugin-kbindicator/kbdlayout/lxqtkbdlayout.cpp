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
#include <QLabel>
#include <QApplication>
#include <QAbstractNativeEventFilter>
#include <QX11Info>
#include <QSettings>

#include <xcb/xcb.h>
#define explicit _explicit
#include <xcb/xkb.h>

#include "lxqtkbdlayout.h"
#include "lxqtkbdkeeper.h"

class LxQtKbdLayoutPrivate: public QAbstractNativeEventFilter
{
public:
    LxQtKbdLayoutPrivate(LxQtKbdLayout *qptr, QSettings * settings):
        q_ptr(qptr),
        m_settings(settings)
    {
        qApp->installNativeEventFilter(this);
    }

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *)
    {
        if (eventType != "xcb_generic_event_t")
            return false;

        xcb_generic_event_t* event = static_cast<xcb_generic_event_t *>(message);
        if ((event->response_type & ~0x80) == m_xkbEventBase + XkbEventCode){
            switch (reinterpret_cast<xcb_xkb_indicator_state_notify_event_t*>(event)->xkbType) {
            case XkbStateNotify:
                m_keeper->updateInfo(reinterpret_cast<xcb_xkb_state_notify_event_t*>(event)->group);
                break;
            case XkbNewKeyboardNotify:
                m_info.readKeyboardInfo();
                emit q_ptr->changed(m_info.currentSym(), m_info.currentName());
                break;
            }
        }

        m_keeper->checkLayout();
        return false;
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

        XkbSelectEvents(disp, XkbUseCoreKbd, XkbStateNotifyMask, XkbStateNotifyMask);

        QString mode = m_settings->value("layout_switch_mode", "application").toString();

        if (mode == "application")
            m_keeper.reset(new LxQtKbdLayoutAppKeeper(m_info));
        else if (mode == "window")
            m_keeper.reset(new LxQtKbdLayoutWinKeeper(m_info));
        else
            m_keeper.reset(new LxQtKbdLayoutKeeper(m_info));

        m_keeper->connect(m_keeper.data(), &LxQtKbdLayoutKeeper::changed, [this](){
            emit q_ptr->changed(m_info.currentSym(), m_info.currentName());
        });

        emit q_ptr->changed(m_info.currentSym(), m_info.currentName());

        return true;
    }

    void switchNext()
    {
        if (m_info.currentGroup() < m_info.size()-1){
            m_keeper->switchLayout(m_info.currentGroup()+1);
        } else {
            m_keeper->switchLayout(0);
        }
    }

    bool readKeyboardInfo()
    { return m_info.readKeyboardInfo(); }

    const LxQtKbdInfo & info() const
    { return m_info; }
private:
    LxQtKbdLayout                       *q_ptr;
    QScopedPointer<LxQtKbdLayoutKeeper>  m_keeper;
    QSettings                           *m_settings;
    int                                  m_xkbEventBase;
    LxQtKbdInfo                          m_info;
};

//--------------------------------------------------------------------------------------------------

LxQtKbdLayout::LxQtKbdLayout(QSettings *settings):
    m_layout(new LxQtKbdLayoutPrivate(this, settings))
{
    m_enabled = false;
    if (m_layout->readKeyboardInfo()){
        m_enabled = true;
        m_widget = new QLabel(m_layout->info().currentSym());
        m_widget->setObjectName("LayoutLabel");
        m_widget->setAlignment(Qt::AlignCenter);
        m_layout->init();
    }
}

LxQtKbdLayout::~LxQtKbdLayout()
{
    delete m_layout;
}

void LxQtKbdLayout::changed(const QString & sym, const QString & title)
{
    m_widget->setText(sym.toUpper());
    m_widget->setToolTip(title);
}

void LxQtKbdLayout::switchNext()
{
    m_layout->switchNext();
}
