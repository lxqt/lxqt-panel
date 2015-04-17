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

#include <QFrame>
#include <QString>
#include <QTimer>
#include <QPointer>
#include "ilxqtpanel.h"
#include "lxqtpanelglobals.h"
#include <LXQt/AddPluginDialog>

class QMenu;
class Plugin;

namespace LxQt {
class Settings;
class PluginInfo;
}
class LxQtPanelLayout;
class ConfigPanelDialog;

/*! \brief The LxQtPanel class provides a single lxqt-panel.
 */
class LXQT_PANEL_API LxQtPanel : public QFrame, public ILxQtPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

    // for configuration dialog
    friend class ConfigPanelWidget;

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
    int panelSize() const { return mPanelSize; }
    int iconSize() const { return mIconSize; }
    int lineCount() const { return mLineCount; }
    int length() const { return mLength; }
    bool lengthInPercents() const { return mLengthInPercents; }
    LxQtPanel::Alignment alignment() const { return mAlignment; }
    int screenNum() const { return mScreenNum; }
    QColor fontColor() const { return mFontColor; };
    QColor backgroundColor() const { return mBackgroundColor; };
    QString backgroundImage() const { return mBackgroundImage; };
    int opacity() const { return mOpacity; };
    bool hidable() const { return mHidable; }

    LxQt::Settings *settings() const { return mSettings; }

public slots:
    void show();
    void showPanel();
    void hidePanel();
    void hidePanelWork();

    // Settings
    void setPanelSize(int value, bool save);
    void setIconSize(int value, bool save);
    void setLineCount(int value, bool save);
    void setLength(int length, bool inPercents, bool save);
    void setPosition(int screen, ILxQtPanel::Position position, bool save);
    void setAlignment(LxQtPanel::Alignment value, bool save);
    void setFontColor(QColor color, bool save);
    void setBackgroundColor(QColor color, bool save);
    void setBackgroundImage(QString path, bool save);
    void setOpacity(int opacity, bool save);
    void setHidable(bool hidable, bool save);

    void saveSettings(bool later=false);
    void ensureVisible();

signals:
    void realigned();
    void deletedByUser(LxQtPanel *self);

    void pluginAdded(LxQt::PluginData const & plugin);
    void pluginRemoved(LxQt::PluginData const & plugin);

protected:
    bool event(QEvent *event);
    void showEvent(QShowEvent *event);

public slots:
    void showConfigDialog();
private slots:
    void addPlugin(const LxQt::PluginInfo &desktopFile);
    void showAddPluginDialog();
    void realign();
    void removePlugin();
    void pluginMoved(Plugin const * plug);
    void userRequestForDeletion();

private:
    LxQtPanelLayout* mLayout;
    LxQt::Settings *mSettings;
    QFrame *LxQtPanelWidget;
    QString mConfigGroup;
    QList<Plugin*> mPlugins;
    QList<QString> mPluginsList;

    int findAvailableScreen(LxQtPanel::Position position);
    void updateWmStrut();

    void loadPlugins();
    Plugin *loadPlugin(const LxQt::PluginInfo &desktopFile, const QString &settingsGroup);
    Plugin *findPlugin(const ILxQtPanelPlugin *iPlugin) const;

    QString findNewPluginSettingsGroup(const QString &pluginType) const;

    int mPanelSize;
    int mIconSize;
    int mLineCount;

    int mLength;
    bool mLengthInPercents;

    Alignment mAlignment;

    ILxQtPanel::Position mPosition;
    int mScreenNum;
    QTimer mDelaySave;
    bool mHidable;
    bool mHidden;
    QTimer mHideTimer;

    QColor mFontColor;
    QColor mBackgroundColor;
    QString mBackgroundImage;
    // 0 to 100
    int mOpacity;
    QPointer<ConfigPanelDialog> mConfigDialog;

    void updateStyleSheet();
};


#endif // LXQTPANEL_H
