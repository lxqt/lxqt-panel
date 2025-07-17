/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QAction>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <lxqt-globalkeys.h>
#include <XdgIcon>
#include <LXQt/Notification>
#include "showdesktop.h"
#include "../panel/pluginsettings.h"

#include "../panel/lxqtpanelapplication.h"
#include "../panel/backends/ilxqtabstractwmiface.h"

#define DEFAULT_SHORTCUT "Control+Alt+D"

ShowDesktop::ShowDesktop(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo)
{
    m_key = GlobalKeyShortcut::Client::instance()->addAction(QString(), QStringLiteral("/panel/%1/show_hide").arg(settings()->group()), tr("Show desktop"), this);
    if (m_key)
    {
        connect(m_key, &GlobalKeyShortcut::Action::registrationFinished, this, &ShowDesktop::shortcutRegistered);
        connect(m_key, &GlobalKeyShortcut::Action::activated,            this, &ShowDesktop::toggleShowingDesktop);
    }

    QAction * act = new QAction(XdgIcon::fromTheme(QStringLiteral("user-desktop")), tr("Show Desktop"), this);
    connect(act, &QAction::triggered, this, &ShowDesktop::toggleShowingDesktop);

    mDNDTimer.setSingleShot(true);
    connect(&mDNDTimer, &QTimer::timeout, this, &ShowDesktop::toggleShowingDesktop, Qt::QueuedConnection);
    mDNDTimer.setInterval(700);

    mButton.setDefaultAction(act);
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mButton.setAutoRaise(true);
    mButton.installEventFilter(this);
    mButton.setAcceptDrops(true);
}

bool ShowDesktop::eventFilter(QObject * watched, QEvent * event)
{
    if (watched == &mButton)
    {
        if (event->type() == QEvent::DragEnter)
        {
            static_cast<QDragEnterEvent *>(event)->acceptProposedAction();
            mDNDTimer.start();
        } else if (event->type() == QEvent::DragLeave)
        {
            mDNDTimer.stop();
        }
        return false;
    }
    return QObject::eventFilter(watched, event);
}

void ShowDesktop::shortcutRegistered()
{
    if (m_key->shortcut().isEmpty())
    {
        m_key->changeShortcut(QStringLiteral(DEFAULT_SHORTCUT));
        if (m_key->shortcut().isEmpty())
        {
            LXQt::Notification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(QStringLiteral(DEFAULT_SHORTCUT)));
        }
    }
}

void ShowDesktop::toggleShowingDesktop()
{
    LXQtPanelApplication *a = reinterpret_cast<LXQtPanelApplication*>(qApp);
    auto wmBackend = a->getWMBackend();
    wmBackend->showDesktop(!wmBackend->isShowingDesktop());
}

#undef DEFAULT_SHORTCUT
