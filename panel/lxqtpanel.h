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

class QMenu;
class Plugin;
class QAbstractItemModel;

namespace LXQt {
class Settings;
class PluginInfo;
}
class LXQtPanelLayout;
class ConfigPanelDialog;
class PanelPluginsModel;

/*! \brief The LXQtPanel class provides a single lxqt-panel.
 */
class LXQT_PANEL_API LXQtPanel : public QFrame, public ILXQtPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

    // for configuration dialog
    friend class ConfigPanelWidget;
    friend class ConfigPluginsWidget;

public:
    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    LXQtPanel(const QString &configGroup, QWidget *parent = 0);
    virtual ~LXQtPanel();

    QString name() { return mConfigGroup; }

    void readSettings();

    void showPopupMenu(Plugin *plugin = 0);

    // ILXQtPanel .........................
    ILXQtPanel::Position position() const { return mPosition; }
    QRect globalGometry() const;
    Plugin *findPlugin(const ILXQtPanelPlugin *iPlugin) const;
    QRect calculatePopupWindowPos(QPoint const & absolutePos, QSize const & windowSize) const;
    QRect calculatePopupWindowPos(const ILXQtPanelPlugin *plugin, const QSize &windowSize) const;

    // For QSS properties ..................
    QString qssPosition() const;

    static bool canPlacedOn(int screenNum, LXQtPanel::Position position);
    static QString positionToStr(ILXQtPanel::Position position);
    static ILXQtPanel::Position strToPosition(const QString &str, ILXQtPanel::Position defaultValue);

    // Settings
    int panelSize() const { return mPanelSize; }
    int iconSize() const { return mIconSize; }
    int lineCount() const { return mLineCount; }
    int length() const { return mLength; }
    bool lengthInPercents() const { return mLengthInPercents; }
    LXQtPanel::Alignment alignment() const { return mAlignment; }
    int screenNum() const { return mScreenNum; }
    QColor fontColor() const { return mFontColor; };
    QColor backgroundColor() const { return mBackgroundColor; };
    QString backgroundImage() const { return mBackgroundImage; };
    int opacity() const { return mOpacity; };
    bool hidable() const { return mHidable; }

    LXQt::Settings *settings() const { return mSettings; }

    bool isPluginSingletonAndRunnig(QString const & pluginId) const;

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
    void setPosition(int screen, ILXQtPanel::Position position, bool save);
    void setAlignment(LXQtPanel::Alignment value, bool save);
    void setFontColor(QColor color, bool save);
    void setBackgroundColor(QColor color, bool save);
    void setBackgroundImage(QString path, bool save);
    void setOpacity(int opacity, bool save);
    void setHidable(bool hidable, bool save);

    void saveSettings(bool later=false);
    void ensureVisible();

signals:
    void realigned();
    void deletedByUser(LXQtPanel *self);
    void pluginAdded();
    void pluginRemoved();

protected:
    bool event(QEvent *event);
    void showEvent(QShowEvent *event);

public slots:
    void showConfigDialog();
private slots:
    void showAddPluginDialog();
    void realign();
    void pluginMoved(Plugin * plug);
    void userRequestForDeletion();

private:
    LXQtPanelLayout* mLayout;
    LXQt::Settings *mSettings;
    QFrame *LXQtPanelWidget;
    QString mConfigGroup;
    QScopedPointer<PanelPluginsModel> mPlugins;

    int findAvailableScreen(LXQtPanel::Position position);
    void updateWmStrut();

    void loadPlugins();

    void setPanelGeometry();
    int getReserveDimension();

    int mPanelSize;
    int mIconSize;
    int mLineCount;

    int mLength;
    bool mLengthInPercents;

    Alignment mAlignment;

    ILXQtPanel::Position mPosition;
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
