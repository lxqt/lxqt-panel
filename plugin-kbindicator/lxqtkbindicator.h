/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#ifndef LXQTPANELKBINDICATOR_H
#define LXQTPANELKBINDICATOR_H

#include "../panel/ilxqtpanelplugin.h"
#include "lxqtkbindicatorconfiguration.h"
#include <KF5/KGuiAddons/KModifierKeyInfo>

class QLabel;

class LxQtKbIndicator : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    LxQtKbIndicator(const ILxQtPanelPluginStartupInfo &startupInfo);
    ~LxQtKbIndicator();

    virtual QWidget *widget();
    virtual QString themeId() const { return "KbIndicator"; }
    virtual ILxQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }
    virtual bool isSeparate() const { return false; }

    QDialog *configureDialog();
    virtual void realign();
    virtual bool eventFilter(QObject *object, QEvent *event);

protected slots:
    void delayedInit();
    virtual void settingsChanged();
    void modifierStateChanged(Qt::Key key, bool active);

private:
    KModifierKeyInfo *modifierInfo;
    QWidget *mContent;

    bool mShowCapsLock;
    bool mShowNumLock;
    bool mShowScrollLock;

    QLabel *mCapsLock;
    QLabel *mNumLock;
    QLabel *mScrollLock;
};

class LxQtKbIndicatorLibrary: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo)
    {
        return new LxQtKbIndicator(startupInfo);
    }
};

#endif // LXQTPANELKBINDICATOR_H
