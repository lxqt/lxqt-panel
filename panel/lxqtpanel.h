/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#ifndef LXQTPANEL_H
#define LXQTPANEL_H

#include <QtGui/QFrame>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include "ilxqtpanel.h"
#include "lxqtpanelglobals.h"

class QMenu;
class Plugin;

namespace LxQt {
class Settings;
class PluginInfo;
}
class LxQtPanelLayout;

/*! \brief The LxQtPanel class provides a single lxqt-panel.
 */
class LXQT_PANEL_API LxQtPanel : public QFrame, public ILxQtPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

public:
    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    LxQtPanel(const QString &configGroup, QWidget *parent = 0);
    virtual ~LxQtPanel();

    QString name() { return mConfigGroup; }

    void readSettings();

    void showPopupMenu(Plugin *plugin = 0);
    void x11EventFilter(XEvent* event);

    // ILxQtPanel .........................
    ILxQtPanel::Position position() const { return mPosition; }
    QRect globalGometry() const;
    QRect calculatePopupWindowPos(const ILxQtPanelPlugin *plugin, const QSize &windowSize) const;

    // For QSS properties ..................
    QString qssPosition() const;

    static bool canPlacedOn(int screenNum, LxQtPanel::Position position);
    static QString positionToStr(ILxQtPanel::Position position);
    static ILxQtPanel::Position strToPosition(const QString &str, ILxQtPanel::Position defaultValue);


    // Settings
    int panelHeight() const { return mPanelHeight; }
    int iconSize() const { return mIconSize; }
    int lineCount() const { return mLineCount; }
    int length() const { return mLength; }
    bool lengthInPercents() const { return mLengthInPercents; }
    LxQtPanel::Alignment alignment() const { return mAlignment; }
    int screenNum() const { return mScreenNum; }

    LxQt::Settings *settings() const { return mSettings; }

public slots:
    void show();

    // Settings
    void setPanelHeight(int value);
    void setIconSize(int value);
    void setLineCount(int value);
    void setLength(int length, bool inPercents);
    void setPosition(int screen, ILxQtPanel::Position position);
    void setAlignment(LxQtPanel::Alignment value);

    void saveSettings(bool later=false);

signals:
    void realigned();
    void deletedByUser(LxQtPanel *self);

protected:
    bool event(QEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void screensChangeds();
    void addPlugin(const LxQt::PluginInfo &desktopFile);
    void showConfigDialog();
    void showAddPluginDialog();
    void realign();
    void removePlugin();
    void pluginMoved();
    void userRequestForDeletion();

private:
    LxQtPanelLayout* mLayout;
    LxQt::Settings *mSettings;
    QFrame *LxQtPanelWidget;
    QString mConfigGroup;
    QList<Plugin*> mPlugins;

    int findAvailableScreen(LxQtPanel::Position position);

    void loadPlugins();
    Plugin *loadPlugin(const LxQt::PluginInfo &desktopFile, const QString &settingsGroup);
    Plugin *findPlugin(const ILxQtPanelPlugin *iPlugin) const;

    QString findNewPluginSettingsGroup(const QString &pluginType) const;

    int mPanelHeight;
    int mIconSize;
    int mLineCount;

    int mLength;
    bool mLengthInPercents;

    LxQtPanel::Alignment mAlignment;

    ILxQtPanel::Position mPosition;
    int mScreenNum;
    QTimer mDelaySave;

    void updateStyleSheet();
};


#endif // LXQTPANEL_H
