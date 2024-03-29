/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2011 Razor team
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


#ifndef DESKTOPSWITCH_H
#define DESKTOPSWITCH_H

#include "../panel/ilxqtpanelplugin.h"
#include <QLabel>
#include <QFrame>
#include <QGuiApplication>
#include <memory>
#include <NETWM>

#include "desktopswitchbutton.h"

class QSignalMapper;
class QButtonGroup;
class NETRootInfo;
namespace LXQt {
class GridLayout;
}

class DesktopSwitchWidget: public QFrame
{
    Q_OBJECT
public:
    DesktopSwitchWidget();

private:
    int m_mouseWheelThresholdCounter;

protected:
    void wheelEvent(QWheelEvent* e);
};

/**
 * @brief Desktop switcher. A very simple one...
 */
class DesktopSwitch : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    DesktopSwitch(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~DesktopSwitch() override;

    QString themeId() const override { return QStringLiteral("DesktopSwitch"); }
    QWidget *widget() override { return &mWidget; }
    bool isSeparate() const override { return true; }
    void realign() override;

    ILXQtPanelPlugin::Flags flags() const override { return HaveConfigDialog; }
    QDialog *configureDialog() override;

private:
    QButtonGroup * m_buttons;
    QList<GlobalKeyShortcut::Action*> m_keys;
    QSignalMapper* m_pSignalMapper;
    int m_desktopCount;
    DesktopSwitchWidget mWidget;
    LXQt::GridLayout *mLayout;
    int mRows;
    bool mShowOnlyActive;
    std::unique_ptr<NETRootInfo> mDesktops;
    DesktopSwitchButton::LabelType mLabelType;

    void refresh();
    bool isWindowHighlightable(WId window);

private slots:
    void setDesktop(int desktop);
    void onNumberOfDesktopsChanged(int);
    void onCurrentDesktopChanged(int);
    void onDesktopNamesChanged();
    void settingsChanged() override;
    void registerShortcuts();
    void shortcutRegistered();
    void onWindowChanged(WId id, NET::Properties properties, NET::Properties2 properties2);
};

class DesktopSwitchUnsupported : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    DesktopSwitchUnsupported(const ILXQtPanelPluginStartupInfo &startupInfo)
        : ILXQtPanelPlugin(startupInfo)
        , mLabel{tr("n/a")}
    {
        mLabel.setToolTip(tr("DesktopSwitch is unsupported on current platform: %1").arg(QGuiApplication::platformName()));
    }

    QString themeId() const { return QStringLiteral("DesktopSwitchUnsupported"); }
    QWidget *widget() { return &mLabel; }
    bool isSeparate() const { return true; }
private:
    QLabel mLabel;
};

class DesktopSwitchPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    // Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        if (QGuiApplication::platformName() == QStringLiteral("xcb"))
            return new DesktopSwitch{startupInfo};
        else
            return new DesktopSwitchUnsupported{startupInfo};
    }
};

#endif
