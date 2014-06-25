/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

#include <QToolButton>
#include <QAction>
#include <QX11Info>
#include <lxqt-globalkeys.h>
#include <XdgIcon>
#include <LXQt/XfitMan>
#include <LXQt/Notification>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "../panel/fixx11h.h" // Xlib headers frequtly causes name clashes. Use this hack to fix it.
#include "showdesktop.h"

using namespace LxQt;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(showdesktop, ShowDesktopLibrary)
#endif

#define DEFAULT_SHORTCUT "Control+Alt+D"


ShowDesktop::ShowDesktop(const ILxQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILxQtPanelPlugin(startupInfo)
{
    m_key = GlobalKeyShortcut::Client::instance()->addAction(QString(), QString("/panel/%1/show_hide").arg(settings()->group()), tr("Show desktop"), this);
    if (m_key)
    {
        connect(m_key, SIGNAL(activated()), this, SLOT(showDesktop()));

        if (m_key->shortcut().isEmpty())
        {
            m_key->changeShortcut(DEFAULT_SHORTCUT);
            if (m_key->shortcut().isEmpty())
            {
                LxQt::Notification::notify(tr("Show Desktop: Global shortcut '%1' cannot be registered").arg(DEFAULT_SHORTCUT));
            }
        }
    }

    QAction * act = new QAction(XdgIcon::fromTheme("user-desktop"), tr("Show Desktop"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(showDesktop()));

    mButton.setDefaultAction(act);
    mButton.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void ShowDesktop::showDesktop()
{
    bool bDesktopShown = xfitMan().getShowingDesktop();
    xfitMan().setShowingDesktop(!bDesktopShown);
}

#undef DEFAULT_SHORTCUT
