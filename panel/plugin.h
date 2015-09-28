/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef PLUGIN_H
#define PLUGIN_H

#include <QFrame>
#include <QString>
#include <LXQt/PluginInfo>
#include <LXQt/Settings>
#include "ilxqtpanel.h"
#include "lxqtpanelglobals.h"
#include "pluginsettings.h"

class QPluginLoader;
class QSettings;
class ILXQtPanelPlugin;
class ILXQtPanelPluginLibrary;
class LXQtPanel;
class QMenu;


class LXQT_PANEL_API Plugin : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(QColor moveMarkerColor READ moveMarkerColor WRITE setMoveMarkerColor)
public:
    enum Alignment {
        AlignLeft,
        AlignRight
    };


    explicit Plugin(const LXQt::PluginInfo &desktopFile, LXQt::Settings *settings, const QString &settingsGroup, LXQtPanel *panel);
    ~Plugin();

    bool isLoaded() const { return mPlugin != 0; }
    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment alignment);

    QString settingsGroup() const { return mSettings->group(); }

    void saveSettings();

    QMenu* popupMenu() const;
    const ILXQtPanelPlugin * iPlugin() const { return mPlugin; }

    const LXQt::PluginInfo desktopFile() const { return mDesktopFile; }

    bool isSeparate() const;
    bool isExpandable() const;

    QWidget *widget() { return mPluginWidget; }

    QString name() const { return mName; }

    // For QSS properties ..................
    static QColor moveMarkerColor() { return mMoveMarkerColor; }
    static void setMoveMarkerColor(QColor color) { mMoveMarkerColor = color; }

public slots:
    void realign();
    void showConfigureDialog();
    void requestRemove();

signals:
    void startMove();
    void remove();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void showEvent(QShowEvent *event);

private:
    bool loadLib(ILXQtPanelPluginLibrary const * pluginLib);
    bool loadModule(const QString &libraryName);
    ILXQtPanelPluginLibrary const * findStaticPlugin(const QString &libraryName);

    const LXQt::PluginInfo mDesktopFile;
    QPluginLoader *mPluginLoader;
    ILXQtPanelPlugin *mPlugin;
    QWidget *mPluginWidget;
    Alignment mAlignment;
    PluginSettings *mSettings;
    LXQtPanel *mPanel;
    static QColor mMoveMarkerColor;
    QString mName;

private slots:
    void settingsChanged();

};


#endif // PLUGIN_H
