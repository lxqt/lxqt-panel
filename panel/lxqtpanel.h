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

/*! \brief The LXQtPanel class provides a single lxqt-panel. All LXQtPanel
 * instances should be created and handled by LXQtPanelApplication. In turn,
 * all Plugins should be created and handled by LXQtPanels.
 *
 * LXQtPanel is just the panel, it does not incorporate any functionality.
 * Each function of the panel is done by Plugins, even the mainmenu
 * (plugin-mainmenu) and the taskbar (plugin-taskbar). So the LXQtPanel is
 * just the container for several Plugins while the different Plugins
 * incorporate the functions of the panel. Without the Plugins, the panel
 * is quite useless.
 *
 * LXQtPanel itself is a window (QFrame/QWidget) and this class is mainly
 * responsible for handling the size and position of this window on the
 * screen(s) as well as the different settings. The handling of the plugins
 * is outsourced in PanelPluginsModel and LXQtPanelLayout. PanelPluginsModel
 * is responsible for loading/creating and handling the plugins.
 * LXQtPanelLayout is inherited from QLayout and set as layout to the
 * background of LXQtPanel, so LXQtPanelLayout is responsible for the
 * layout of all the Plugins.
 *
 * \sa LXQtPanelApplication, Plugin, PanelPluginsModel, LXQtPanelLayout.
 */
class LXQT_PANEL_API LXQtPanel : public QFrame, public ILXQtPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

    // for configuration dialog
    friend class ConfigPanelWidget;
    friend class ConfigPluginsWidget;

public:
    /**
     * @brief Stores how the panel should be aligned. Obviously, this applies
     * only if the panel does not occupy 100 % of the available space. If the
     * panel is vertical, AlignmentLeft means align to the top border of the
     * screen, AlignmentRight means align to the bottom.
     */
    enum Alignment {
        AlignmentLeft   = -1, //!< Align the panel to the left or top
        AlignmentCenter =  0, //!< Center the panel
        AlignmentRight  =  1 //!< Align the panel to the right or bottom
    };

    /**
     * @brief Creates and initializes the LXQtPanel. Performs the following
     * steps:
     * 1. Sets Qt window title, flags, attributes.
     * 2. Creates the panel layout.
     * 3. Prepares the timers.
     * 4. Connects signals and slots.
     * 5. Gets the LXQt::Settings from LXQtPanelApplication.
     * 6. Optionally moves the panel to a valid screen (position-dependent).
     * 7. Loads the Plugins.
     * 8. Shows the panel, even if it is hidable (but then, starts the timer).
     * @param configGroup The name of the panel which is used as identifier
     * in the config file.
     * @param parent Parent QWidget, can be omitted.
     */
    LXQtPanel(const QString &configGroup, QWidget *parent = 0);
    virtual ~LXQtPanel();

    /**
     * @brief Returns the name of this panel which is also used as identifier
     * in the config file.
     * @return The name of this panel.
     */
    QString name() { return mConfigGroup; }

    /**
     * @brief Reads all the necessary settings from mSettings and stores them
     * in local variables. Already calls necessary methods like realign() or
     * updateStyleSheet() which need to get called after changing settings.
     */
    void readSettings();

    /**
     * @brief Creates and shows the popup menu (right click menu). If a plugin
     * is given as parameter, the menu will be divided in two groups:
     * plugin-specific options and panel-related options. As these two are
     * shown together, this menu has to be created by LXQtPanel.
     * @param plugin
     */
    void showPopupMenu(Plugin *plugin = 0);

    // ILXQtPanel .........................
    /**
     * @brief Implements ILXQtPanel::position().
     * @return
     */
    ILXQtPanel::Position position() const { return mPosition; }
    /**
     * @brief Implements ILXQtPanel::globalGometry().
     * @return
     */
    QRect globalGometry() const;
    /**
     * @brief Implements ILXQtPanel::calculatePopupWindowPos().
     * @param absolutePos
     * @param windowSize
     * @return
     */
    QRect calculatePopupWindowPos(QPoint const & absolutePos, QSize const & windowSize) const;
    /**
     * @brief ILXQtPanel::calculatePopupWindowPos().
     * @param plugin
     * @param windowSize
     * @return
     */
    QRect calculatePopupWindowPos(const ILXQtPanelPlugin *plugin, const QSize &windowSize) const;

    /**
     * @brief Searches for a Plugin in the Plugins-list of this panel. Takes
     * an ILXQtPanelPlugin as parameter and returns the corresponding Plugin.
     * @param iPlugin ILXQtPanelPlugin that we are looking for.
     * @return The corresponding Plugin if it is loaded in this panel, nullptr
     * otherwise.
     */
    Plugin *findPlugin(const ILXQtPanelPlugin *iPlugin) const;

    // For QSS properties ..................
    /**
     * @brief Returns the position as string
     * @return The position as string.
     *
     * \sa positionToStr().
     */
    QString qssPosition() const;

    /**
     * @brief Checks if this LXQtPanel can be placed on a given position
     * on the screen with the given screenNum. The condition for doing so
     * is that the panel is not located between two screens.
     *
     * For example, if position is PositionRight, there should be no screen to
     * the right of the given screen. That means that there should be no
     * screen whose left border has a higher x-coordinate than the x-coordinate
     * of the right border of the given screen. This method iterates over all
     * screens and checks these conditions.
     * @param screenNum screen index as it is used by QDesktopWidget methods
     * @param position position where the panel should be placed
     * @return true if this panel can be placed on the given position on the
     * given screen.
     */
    static bool canPlacedOn(int screenNum, LXQtPanel::Position position);
    /**
     * @brief Returns a string representation of the given position. This
     * string is human-readable and can be used in config files.
     * @param position position that should be converted to a string.
     * @return the string representation of the given position, i.e.
     * "Top", "Left", "Right" or "Bottom".
     *
     * \sa strToPosition()
     */
    static QString positionToStr(ILXQtPanel::Position position);
    /**
     * @brief Returns an ILXQtPanel::Position from the given string. This can
     * be used to retrieve ILXQtPanel::Position values from the config files.
     * @param str string that should be converted to ILXQtPanel::Position
     * @param defaultValue value that will be returned if the string can not
     * be converted to an ILXQtPanel::Position.
     * @return ILXQtPanel::Position that was determined from str or
     * defaultValue if str could not be converted.
     *
     * \sa positionToStr()
     */
    static ILXQtPanel::Position strToPosition(const QString &str, ILXQtPanel::Position defaultValue);

    // Settings
    /**
     * @brief Implements ILXQtPanel::iconSize().
     * @return
     */
    int iconSize() const { return mIconSize; }
    /**
     * @brief Implements ILXQtPanel::lineCount().
     * @return
     */
    int lineCount() const { return mLineCount; }
    /**
     * @brief Returns the panel size in pixels, i.e. the height of a
     * horizontal panel or the width of a vertical panel.
     * @return The panel size.
     *
     * \sa mPanelSize.
     */
    int panelSize() const { return mPanelSize; }
    /**
     * @brief Returns the length of the panel. The unit of this value depends
     * on lengthInPercents().
     * @return The length of the panel.
     *
     * \sa mLength, lengthInPercents().
     */
    int length() const { return mLength; }
    /**
     * @brief Determines if the length of the panel is stored in pixels or
     * relative in percents to the screen size.
     * @return true if the length is given in percents, false if it is given
     * in pixels.
     *
     * \sa mLengthInPercents, length().
     */
    bool lengthInPercents() const { return mLengthInPercents; }
    /**
     * @brief Returns the LXQtPanel::Alignment of this panel. The meaning of
     * this value differs for horizontal and vertical panels.
     *
     * \sa LXQtPanel::Alignment, mAlignment.
     */
    LXQtPanel::Alignment alignment() const { return mAlignment; }
    /**
     * @brief Returns the index of the screen on which this panel should be
     * shown. This is the user configured value which can differ from the
     * screen that the panel is actually shown on. If the panel can not be
     * shown on the configured screen, LXQtPanel will determine another
     * screen. The screen that the panel is actually shown on is stored in
     * mActualScreenNum.
     *
     * @return The index of the screen on which this panel should be shown.
     *
     * \sa mScreenNum, mActualScreenNum, canPlacedOn(), findAvailableScreen().
     */
    int screenNum() const { return mScreenNum; }
    QColor fontColor() const { return mFontColor; } //!< Returns the font color for the panel.
    QColor backgroundColor() const { return mBackgroundColor; } //!< Returns the background color for the panel.
    QString backgroundImage() const { return mBackgroundImage; } //!< Returns the name of the background image.
    /**
     * @brief Returns the opacity of the background color. This value does
     * not affect the opacity of the background image. The returned value is
     * in the range 0 to 100.
     * @return The opacity in the range 0 to 100.
     */
    int opacity() const { return mOpacity; }
    bool hidable() const { return mHidable; } //!< Returns if the panel is hidable, i.e. if the panel will be hidden when the cursor leaves the panel area.

    LXQt::Settings *settings() const { return mSettings; } //!< Returns the LXQt::Settings instance as retrieved from LXQtPanelApplication.

    /*!
     * \brief Checks if a given Plugin is running and has the
     * ILXQtPanelPlugin::SingleInstance flag set.
     * \param pluginId TODO
     * \return true if the Plugin is running and has the
     * ILXQtPanelPlugin::SingleInstance flag set, false otherwise.
     */
    bool isPluginSingletonAndRunnig(QString const & pluginId) const;

public slots:
    /**
     * @brief Shows the QWidget and makes it visible on all desktops. This
     * method is NOT related to showPanel(), hidePanel() and hidePanelWork()
     * which handle the LXQt hiding by resizing the panel.
     */
    void show();
    /**
     * @brief Shows the panel (immediately) after it had been hidden before.
     * Stops the QTimer mHideTimer. This it NOT the same as QWidget::show()
     * because hiding the panel in LXQt is done by making it very thin. So
     * this method in fact restores the original size of the panel.
     *
     * \sa mHidable, mHidden, mHideTimer, hidePanel(), hidePanelWork()
     */
    void showPanel();
    /**
     * @brief Hides the panel (delayed) by starting the QTimer mHideTimer.
     * When this timer times out, hidePanelWork() will be called. So this
     * method is called when the cursor leaves the panel area but the panel
     * will be hidden later.
     *
     * \sa mHidable, mHidden, mHideTimer, showPanel(), hidePanelWork()
     */
    void hidePanel();
    /**
     * @brief Actually hides the panel. Will be invoked when the QTimer
     * mHideTimer times out. That timer will be started by showPanel(). This
     * is NOT the same as QWidget::hide() because hiding the panel in LXQt is
     * done by making the panel very thin. So this method in fact makes the
     * panel very thin while the QWidget stays visible.
     *
     * \sa mHidable, mHidden, mHideTimer, showPanel(), hidePanel()
     */
    void hidePanelWork();

    // Settings
    /**
     * @brief All the setter methods are  designed similar:
     * 1. Check if the given value is different from the current value. If not,
     * do not do anything and return.
     * 2. Set the value.
     * 3. If parameter save is true, call saveSettings(true) to store the
     * new settings on the disk.
     * 4. If necessary, propagate the new value to child objects, e.g. to
     * mLayout.
     * 5. If necessary, call update methods like realign() or
     * updateStyleSheet().
     * @param value The value that should be set.
     * @param save If true, saveSettings(true) will be called.
     */
    void setPanelSize(int value, bool save);
    void setIconSize(int value, bool save); //!< \sa setPanelSize()
    void setLineCount(int value, bool save); //!< \sa setPanelSize()
    void setLength(int length, bool inPercents, bool save); //!< \sa setPanelSize()
    void setPosition(int screen, ILXQtPanel::Position position, bool save); //!< \sa setPanelSize()
    void setAlignment(LXQtPanel::Alignment value, bool save); //!< \sa setPanelSize()
    void setFontColor(QColor color, bool save); //!< \sa setPanelSize()
    void setBackgroundColor(QColor color, bool save); //!< \sa setPanelSize()
    void setBackgroundImage(QString path, bool save); //!< \sa setPanelSize()
    void setOpacity(int opacity, bool save); //!< \sa setPanelSize()
    void setHidable(bool hidable, bool save); //!< \sa setPanelSize()

    /**
     * @brief Saves the current configuration, i.e. writes the current
     * configuration varibles to the LXQt::Settings object mSettings.
     * @param later Determines if the settings are written immediately or
     * after a short delay. If later==true, the QTimer mDelaySave is started.
     * As soon as this timer times out, saveSettings(false) will be called. If
     * later==false, settings will be written.
     */
    void saveSettings(bool later=false);
    /**
     * @brief Checks if the panel can be placed on the current screen at the
     * current position. If it can not, it will be moved on another screen
     * where the desired position is possible.
     */
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
    /**
     * @brief The LXQtPanelLayout of this panel. All the Plugins will be added
     * to the UI via this layout.
     */
    LXQtPanelLayout* mLayout;
    /**
     * @brief The LXQt::Settings instance as retrieved from
     * LXQtPanelApplication.
     */
    LXQt::Settings *mSettings;
    /**
     * @brief The background widget for the panel. This background widget will
     * have the background color or the background image if any of these is
     * set. This background widget will have the LXQtPanelLayout mLayout which
     * will in turn contain all the Plugins.
     */
    QFrame *LXQtPanelWidget;
    /**
     * @brief The name of the panel which will also be used as an identifier
     * for config files. That's why it is called like that.
     */
    QString mConfigGroup;
    /**
     * @brief Pointer to the PanelPluginsModel which will store all the Plugins
     * that are loaded.
     */
    QScopedPointer<PanelPluginsModel> mPlugins;

    /**
     * @brief Returns the screen index of a screen on which this panel could
     * be placed on the given position. If possible, the current screen index
     * is preserved. So, if the panel can be placed on the current screen, the
     * index of that screen will be returned.
     * @param position position on which the panel should be placed.
     * @return The current screen index if the panel can be placed on the
     * current screen or the screen index of a screen that it can be placed on.
     */
    int findAvailableScreen(LXQtPanel::Position position);
    /**
     * @brief Update the _NET_WM_PARTIAL_STRUT and _NET_WM_STRUT properties
     * for the window.
     */
    void updateWmStrut();

    /**
     * @brief Loads the plugins, i.e. creates a new PanelPluginsModel.
     * Connects the signals and slots and adds all the plugins to the
     * layout.
     */
    void loadPlugins();

    /**
     * @brief Calculates and sets the geometry (i.e. the position and the size
     * on the screen) of the panel. Considers alignment, position and if the
     * panel is hidden.
     */
    void setPanelGeometry();
    /**
     * @brief Calculates the height of the panel if it is horizontal or the
     * width if the panel is vertical. Considers if the panel is hidden and
     * ensures that the result is at least PANEL_MINIMUM_SIZE.
     * @return The height/width of the panel.
     */
    int getReserveDimension();

    /**
     * @brief Stores the size of the panel, i.e. the height of a horizontal
     * panel or the width of a vertical panel in pixels. If the panel is
     * hidden (which is achieved by making the panel very thin), this value
     * is unchanged. So this value stores the size of the non-hidden panel.
     *
     * \sa panelSize(), setPanelSize().
     */
    int mPanelSize;
    /**
     * @brief Stores the edge length of the panel icons in pixels.
     *
     * \sa ILXQtPanel::iconSize(), setIconSize().
     */
    int mIconSize;
    /**
     * @brief Stores the number of lines/rows of the panel.
     *
     * \sa ILXQtPanel::lineCount(), setLineCount().
     */
    int mLineCount;

    /**
     * @brief Stores the length of the panel, i.e. the width of a horizontal
     * panel or the height of a vertical panel. The unit of this value is
     * determined by mLengthInPercents.
     *
     * \sa mLengthInPercents, lengthInPercents(), length(), setLength().
     */
    int mLength;
    /**
     * @brief Stores if the length of the panel (horizontal -> width,
     * vertical -> height) is stored in pixels or relativ in percents
     * to the screen size. If true, the length is stored in percents,
     * otherwise in pixels.
     */
    bool mLengthInPercents;

    /**
     * @brief Stores how this panel is aligned. The meaning of this value
     * differs for horizontal and vertical panels.
     *
     * \sa Alignment.
     */
    Alignment mAlignment;

    ILXQtPanel::Position mPosition; //!< Stores the position where the panel is shown
    int mScreenNum; //!< configured screen (user preference)
    int mActualScreenNum; //!< panel currently shown at (if the configured screen is not available)
    /**
     * @brief QTimer for delayed saving of changed settings. In many cases,
     * instead of storing changes to disk immediately we start this timer.
     * If this timer times out, we store the changes to disk. This has the
     * advantage that we can store a couple of changes with only one write to
     * disk.
     *
     * \sa saveSettings()
     */
    QTimer mDelaySave;
    /**
     * @brief Stores the current user setting if the panel should automatically
     * be hidden after a short delay after the cursor has left the panel area.
     *
     * \sa mHidden, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    bool mHidable;
    /**
     * @brief Stores if the panel is currently hidden.
     *
     * \sa mHidable, mHideTimer, showPanel(), hidePanel(), hidePanelWork()
     */
    bool mHidden;
    /**
     * @brief QTimer for hiding the panel. When the cursor leaves the panel
     * area, this timer will be started. After this timer has timed out, the
     * panel will actually be hidden.
     *
     * \sa mHidable, mHidden, showPanel(), hidePanel(), hidePanelWork()
     */
    QTimer mHideTimer;

    QColor mFontColor; //!< Font color that is used in the style sheet.
    QColor mBackgroundColor; //!< Background color that is used in the style sheet.
    QString mBackgroundImage; //!< Background image that is used in the style sheet.
    /**
     * @brief Determines the opacity of the background color. The value
     * should be in the range from 0 to 100. This will not affect the opacity
     * of a background image.
     */
    int mOpacity;

    /**
     * @brief Pointer to the current ConfigPanelDialog if there is any. Make
     * sure to test this pointer for validity because it is lazily loaded.
     */
    QPointer<ConfigPanelDialog> mConfigDialog;

    /**
     * @brief Updates the style sheet for the panel. First, the stylesheet is
     * created from the preferences. Then, it is set via
     * QWidget::setStyleSheet().
     */
    void updateStyleSheet();
};


#endif // LXQTPANEL_H
