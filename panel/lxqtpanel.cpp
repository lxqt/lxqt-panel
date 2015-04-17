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


#include "lxqtpanel.h"
#include "lxqtpanellimits.h"
#include "ilxqtpanelplugin.h"
#include "lxqtpanelapplication.h"
#include "lxqtpanellayout.h"
#include "config/configpaneldialog.h"
#include "popupmenu.h"
#include "plugin.h"
#include <LXQt/Settings>
#include <LXQt/PluginInfo>

#include <QScreen>
#include <QWindow>
#include <QX11Info>
#include <QDebug>
#include <QString>
#include <QDesktopWidget>
#include <QMenu>
#include <XdgIcon>
#include <XdgDirs>

#include <KWindowSystem/KWindowSystem>
#include <KWindowSystem/NETWM>

// Turn on this to show the time required to load each plugin during startup
// #define DEBUG_PLUGIN_LOADTIME
#ifdef DEBUG_PLUGIN_LOADTIME
#include <QElapsedTimer>
#endif

// Config keys and groups
#define CFG_KEY_SCREENNUM          "desktop"
#define CFG_KEY_POSITION           "position"
#define CFG_KEY_PANELSIZE          "panelSize"
#define CFG_KEY_ICONSIZE           "iconSize"
#define CFG_KEY_LINECNT            "lineCount"
#define CFG_KEY_LENGTH             "width"
#define CFG_KEY_PERCENT            "width-percent"
#define CFG_KEY_ALIGNMENT          "alignment"
#define CFG_KEY_FONTCOLOR          "font-color"
#define CFG_KEY_BACKGROUNDCOLOR    "background-color"
#define CFG_KEY_BACKGROUNDIMAGE    "background-image"
#define CFG_KEY_OPACITY            "opacity"
#define CFG_KEY_PLUGINS            "plugins"
#define CFG_KEY_HIDABLE            "hidable"

/************************************************
 Returns the Position by the string.
 String is one of "Top", "Left", "Bottom", "Right", string is not case sensitive.
 If the string is not correct, returns defaultValue.
 ************************************************/
ILxQtPanel::Position LxQtPanel::strToPosition(const QString& str, ILxQtPanel::Position defaultValue)
{
    if (str.toUpper() == "TOP")    return LxQtPanel::PositionTop;
    if (str.toUpper() == "LEFT")   return LxQtPanel::PositionLeft;
    if (str.toUpper() == "RIGHT")  return LxQtPanel::PositionRight;
    if (str.toUpper() == "BOTTOM") return LxQtPanel::PositionBottom;
    return defaultValue;
}


/************************************************
 Return  string representation of the position
 ************************************************/
QString LxQtPanel::positionToStr(ILxQtPanel::Position position)
{
    switch (position)
    {
    case LxQtPanel::PositionTop:
        return QString("Top");
    case LxQtPanel::PositionLeft:
        return QString("Left");
    case LxQtPanel::PositionRight:
        return QString("Right");
    case LxQtPanel::PositionBottom:
        return QString("Bottom");
    }

    return QString();
}


/************************************************

 ************************************************/
LxQtPanel::LxQtPanel(const QString &configGroup, QWidget *parent) :
    QFrame(parent),
    mConfigGroup(configGroup),
    mPanelSize(0),
    mIconSize(0),
    mLineCount(0),
    mLength(0),
    mAlignment(AlignmentLeft),
    mPosition(ILxQtPanel::PositionBottom),
    mScreenNum(0), //whatever (avoid conditional on uninitialized value)
    mHidable(false),
    mHidden(false)
{
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;

    // NOTE: by PCMan:
    // In Qt 4, the window is not activated if it has Qt::WA_X11NetWmWindowTypeDock.
    // Since Qt 5, the default behaviour is changed. A window is always activated on mouse click.
    // Please see the source code of Qt5: src/plugins/platforms/xcb/qxcbwindow.cpp.
    // void QXcbWindow::handleButtonPressEvent(const xcb_button_press_event_t *event)
    // This new behaviour caused lxqt bug #161 - Cannot minimize windows from panel 1 when two task managers are open
    // Besides, this breaks minimizing or restoring windows when clicking on the taskbar buttons.
    // To workaround this regression bug, we need to add this window flag here.
    // However, since the panel gets no keyboard focus, this may decrease accessibility since
    // it's not possible to use the panel with keyboards. We need to find a better solution later.
    flags |= Qt::WindowDoesNotAcceptFocus;

    setWindowFlags(flags);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AcceptDrops);

    setWindowTitle("LxQt Panel");
    setObjectName(QString("LxQtPanel %1").arg(configGroup));

    LxQtPanelWidget = new QFrame(this);
    LxQtPanelWidget->setObjectName("BackgroundWidget");
    QGridLayout* lav = new QGridLayout();
    lav->setMargin(0);
    setLayout(lav);
    this->layout()->addWidget(LxQtPanelWidget);

    mLayout = new LxQtPanelLayout(LxQtPanelWidget);
    connect(mLayout, SIGNAL(pluginMoved(Plugin const *)), this, SLOT(pluginMoved(Plugin const *)));
    LxQtPanelWidget->setLayout(mLayout);
    mLayout->setLineCount(mLineCount);

    mDelaySave.setSingleShot(true);
    mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
    connect(&mDelaySave, SIGNAL(timeout()), this, SLOT(saveSettings()));

    mHideTimer.setSingleShot(true);
    mHideTimer.setInterval(PANEL_HIDE_DELAY);
    connect(&mHideTimer, SIGNAL(timeout()), this, SLOT(hidePanelWork()));

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(realign()));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(ensureVisible()));
    connect(LxQt::Settings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(lxqtApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    LxQtPanelApplication *app = reinterpret_cast<LxQtPanelApplication*>(qApp);
    mSettings = app->settings();
    readSettings();
    mHidden = false;//overriding configuration to show it first time
    QTimer::singleShot(PANEL_HIDE_FIRST_TIME, this, SLOT(hidePanel()));
    // the old position might be on a visible screen
    ensureVisible();
    loadPlugins();

    show();
}

/************************************************

 ************************************************/
void LxQtPanel::readSettings()
{
    // Read settings ......................................
    mSettings->beginGroup(mConfigGroup);

    // By default we are using size & count from theme.
    setPanelSize(mSettings->value(CFG_KEY_PANELSIZE, PANEL_DEFAULT_SIZE).toInt(), false);
    setIconSize(mSettings->value(CFG_KEY_ICONSIZE, PANEL_DEFAULT_ICON_SIZE).toInt(), false);
    setLineCount(mSettings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt(), false);

    setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
              mSettings->value(CFG_KEY_PERCENT, true).toBool(),
              false);

    setPosition(mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt(),
                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom),
                false);

    setAlignment(Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()), false);

    QColor color = mSettings->value(CFG_KEY_FONTCOLOR, "").value<QColor>();
    if (color.isValid())
        setFontColor(color, true);

    setOpacity(mSettings->value(CFG_KEY_OPACITY, 100).toInt(), true);
    color = mSettings->value(CFG_KEY_BACKGROUNDCOLOR, "").value<QColor>();
    if (color.isValid())
        setBackgroundColor(color, true);

    QString image = mSettings->value(CFG_KEY_BACKGROUNDIMAGE, "").toString();
    if (!image.isEmpty())
        setBackgroundImage(image, false);

    mHidable = mSettings->value(CFG_KEY_HIDABLE, mHidable).toBool();
    mHidden = mHidable;

    mSettings->endGroup();
}


/************************************************

 ************************************************/
void LxQtPanel::saveSettings(bool later)
{
    mDelaySave.stop();
    if (later)
    {
        mDelaySave.start();
        return;
    }

    mSettings->beginGroup(mConfigGroup);

    mSettings->setValue(CFG_KEY_PLUGINS, (mPluginsList.isEmpty() ? "" : QVariant(mPluginsList)));

    mSettings->setValue(CFG_KEY_PANELSIZE, mPanelSize);
    mSettings->setValue(CFG_KEY_ICONSIZE, mIconSize);
    mSettings->setValue(CFG_KEY_LINECNT, mLineCount);

    mSettings->setValue(CFG_KEY_LENGTH, mLength);
    mSettings->setValue(CFG_KEY_PERCENT, mLengthInPercents);

    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);

    mSettings->setValue(CFG_KEY_FONTCOLOR, mFontColor.isValid() ? mFontColor : QColor());
    mSettings->setValue(CFG_KEY_BACKGROUNDCOLOR, mBackgroundColor.isValid() ? mBackgroundColor : QColor());
    mSettings->setValue(CFG_KEY_BACKGROUNDIMAGE, QFileInfo(mBackgroundImage).exists() ? mBackgroundImage : QString());
    mSettings->setValue(CFG_KEY_OPACITY, mOpacity);

    mSettings->setValue(CFG_KEY_HIDABLE, mHidable);

    mSettings->endGroup();
}


/************************************************

 ************************************************/
void LxQtPanel::ensureVisible()
{
    if (!canPlacedOn(mScreenNum, mPosition))
        setPosition(findAvailableScreen(mPosition), mPosition, true);

    // the screen size might be changed, let's update the reserved screen space.
    updateWmStrut();
}


/************************************************

 ************************************************/
LxQtPanel::~LxQtPanel()
{
    mLayout->setEnabled(false);
    // do not save settings because of "user deleted panel" functionality saveSettings();
    qDeleteAll(mPlugins);
}


/************************************************

 ************************************************/
void LxQtPanel::show()
{
    QWidget::show();
    KWindowSystem::setOnDesktop(effectiveWinId(), NET::OnAllDesktops);
}


/************************************************

 ************************************************/
QStringList pluginDesktopDirs()
{
    QStringList dirs;
    dirs << QString(getenv("LXQT_PANEL_PLUGINS_DIR")).split(':', QString::SkipEmptyParts);
    dirs << QString("%1/%2").arg(XdgDirs::dataHome(), "/lxqt/lxqt-panel");
    dirs << PLUGIN_DESKTOPS_DIR;
    return dirs;
}


/************************************************

 ************************************************/
void LxQtPanel::loadPlugins()
{
    QStringList desktopDirs = pluginDesktopDirs();
    mSettings->beginGroup(mConfigGroup);
    mPluginsList = mSettings->value(CFG_KEY_PLUGINS).toStringList();
    mSettings->endGroup();

#ifdef DEBUG_PLUGIN_LOADTIME
    QElapsedTimer timer;
    timer.start();
    qint64 lastTime = 0;
#endif
    foreach (QString sect, mPluginsList)
    {
        QString type = mSettings->value(sect+"/type").toString();
        if (type.isEmpty())
        {
            qWarning() << QString("Section \"%1\" not found in %2.").arg(sect, mSettings->fileName());
            continue;
        }

        LxQt::PluginInfoList list = LxQt::PluginInfo::search(desktopDirs, "LxQtPanel/Plugin", QString("%1.desktop").arg(type));
        if( !list.count())
        {
            qWarning() << QString("Plugin \"%1\" not found.").arg(type);
            continue;
        }

        loadPlugin(list.first(), sect);
#ifdef DEBUG_PLUGIN_LOADTIME
        qDebug() << "load plugin" << type << "takes" << (timer.elapsed() - lastTime) << "ms";
        lastTime = timer.elapsed();
#endif
    }
}


/************************************************

 ************************************************/
Plugin *LxQtPanel::loadPlugin(const LxQt::PluginInfo &desktopFile, const QString &settingsGroup)
{
    Plugin *plugin = new Plugin(desktopFile, mSettings->fileName(), settingsGroup, this);
    if (plugin->isLoaded())
    {
        mPlugins.append(plugin);
        connect(plugin, SIGNAL(startMove()), mLayout, SLOT(startMovePlugin()));
        connect(plugin, SIGNAL(remove()), this, SLOT(removePlugin()));
        connect(this, SIGNAL(realigned()), plugin, SLOT(realign()));
        mLayout->addWidget(plugin);
        return plugin;
    }

    delete plugin;
    return 0;
}


/************************************************

 ************************************************/
void LxQtPanel::realign()
{
    if (!isVisible())
        return;
#if 0
    qDebug() << "** Realign *********************";
    qDebug() << "PanelSize:   " << mPanelSize;
    qDebug() << "IconSize:      " << mIconSize;
    qDebug() << "LineCount:     " << mLineCount;
    qDebug() << "Length:        " << mLength << (mLengthInPercents ? "%" : "px");
    qDebug() << "Alignment:     " << (mAlignment == 0 ? "center" : (mAlignment < 0 ? "left" : "right"));
    qDebug() << "Position:      " << positionToStr(mPosition) << "on" << mScreenNum;
    qDebug() << "Plugins count: " << mPlugins.count();
#endif

    const QRect currentScreen = QApplication::desktop()->screenGeometry(mScreenNum);
    QRect rect;

    if (isHorizontal())
    {
        // Horiz panel ***************************
        rect.setHeight(mHidden ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, mPanelSize));
        if (mLengthInPercents)
            rect.setWidth(currentScreen.width() * mLength / 100.0);
        else
        {
            if (mLength <= 0)
                rect.setWidth(currentScreen.width() + mLength);
            else
                rect.setWidth(mLength);
        }

        rect.setWidth(qMax(rect.size().width(), mLayout->minimumSize().width()));

        // Horiz ......................
        switch (mAlignment)
        {
        case LxQtPanel::AlignmentLeft:
            rect.moveLeft(currentScreen.left());
            break;

        case LxQtPanel::AlignmentCenter:
            rect.moveCenter(currentScreen.center());
            break;

        case LxQtPanel::AlignmentRight:
            rect.moveRight(currentScreen.right());
            break;
        }

        // Vert .......................
        if (mPosition == ILxQtPanel::PositionTop)
            rect.moveTop(currentScreen.top());
        else
            rect.moveBottom(currentScreen.bottom());
    }
    else
    {
        // Vert panel ***************************
        rect.setWidth(mHidden ? PANEL_HIDE_SIZE : qMax(PANEL_MINIMUM_SIZE, mPanelSize));
        if (mLengthInPercents)
            rect.setHeight(currentScreen.height() * mLength / 100.0);
        else
        {
            if (mLength <= 0)
                rect.setHeight(currentScreen.height() + mLength);
            else
                rect.setHeight(mLength);
        }

        rect.setHeight(qMax(rect.size().height(), mLayout->minimumSize().height()));

        // Vert .......................
        switch (mAlignment)
        {
        case LxQtPanel::AlignmentLeft:
            rect.moveTop(currentScreen.top());
            break;

        case LxQtPanel::AlignmentCenter:
            rect.moveCenter(currentScreen.center());
            break;

        case LxQtPanel::AlignmentRight:
            rect.moveBottom(currentScreen.bottom());
            break;
        }

        // Horiz ......................
        if (mPosition == ILxQtPanel::PositionLeft)
            rect.moveLeft(currentScreen.left());
        else
            rect.moveRight(currentScreen.right());
    }
    mLayout->setMargin(mHidden ? PANEL_HIDE_MARGIN : 0);
    if (rect != geometry())
    {
        setGeometry(rect);
        setFixedSize(rect.size());
    }

    // Reserve our space on the screen ..........
    // It's possible that our geometry is not changed, but screen resolution is changed,
    // so resetting WM_STRUT is still needed. To make it simple, we always do it.
    updateWmStrut();
}


// Update the _NET_WM_PARTIAL_STRUT and _NET_WM_STRUT properties for the window
void LxQtPanel::updateWmStrut()
{
    WId wid = effectiveWinId();
    if(wid == 0 || !isVisible())
        return;

    const QRect wholeScreen = QApplication::desktop()->geometry();
    // qDebug() << "wholeScreen" << wholeScreen;
    const QRect rect = geometry();
    // NOTE: http://standards.freedesktop.org/wm-spec/wm-spec-latest.html
    // Quote from the EWMH spec: " Note that the strut is relative to the screen edge, and not the edge of the xinerama monitor."
    // So, we use the geometry of the whole screen to calculate the strut rather than using the geometry of individual monitors.
    // Though the spec only mention Xinerama and did not mention XRandR, the rule should still be applied.
    // At least openbox is implemented like this.
    switch (mPosition)
    {
    case LxQtPanel::PositionTop:
        KWindowSystem::setExtendedStrut(wid,
                                        /* Left   */  0, 0, 0,
                                        /* Right  */  0, 0, 0,
                                        /* Top    */  height(), rect.left(), rect.right(),
                                        /* Bottom */  0, 0, 0
                                       );
        break;

    case LxQtPanel::PositionBottom:
        KWindowSystem::setExtendedStrut(wid,
                                        /* Left   */  0, 0, 0,
                                        /* Right  */  0, 0, 0,
                                        /* Top    */  0, 0, 0,
                                        /* Bottom */  wholeScreen.bottom() - rect.y(), rect.left(), rect.right()
                                       );
        break;

    case LxQtPanel::PositionLeft:
        KWindowSystem::setExtendedStrut(wid,
                                        /* Left   */  width(), rect.top(), rect.bottom(),
                                        /* Right  */  0, 0, 0,
                                        /* Top    */  0, 0, 0,
                                        /* Bottom */  0, 0, 0
                                       );

        break;

    case LxQtPanel::PositionRight:
        KWindowSystem::setExtendedStrut(wid,
                                        /* Left   */  0, 0, 0,
                                        /* Right  */  wholeScreen.right() - rect.x(), rect.top(), rect.bottom(),
                                        /* Top    */  0, 0, 0,
                                        /* Bottom */  0, 0, 0
                                       );
        break;
    }
}


/************************************************
  The panel can't be placed on boundary of two displays.
  This function checks, is the panel can be placed on the display
  @displayNum on @position.
 ************************************************/
bool LxQtPanel::canPlacedOn(int screenNum, LxQtPanel::Position position)
{
    QDesktopWidget* dw = QApplication::desktop();

    switch (position)
    {
    case LxQtPanel::PositionTop:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).bottom() < dw->screenGeometry(screenNum).top())
                return false;
        return true;

    case LxQtPanel::PositionBottom:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).top() > dw->screenGeometry(screenNum).bottom())
                return false;
        return true;

    case LxQtPanel::PositionLeft:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
                return false;
        return true;

    case LxQtPanel::PositionRight:
        for (int i = 0; i < dw->screenCount(); ++i)
            if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
                return false;
        return true;
    }

    return false;
}


/************************************************

 ************************************************/
int LxQtPanel::findAvailableScreen(LxQtPanel::Position position)
{
    int current = mScreenNum;

    for (int i = current; i < QApplication::desktop()->screenCount(); ++i)
        if (canPlacedOn(i, position))
            return i;

    for (int i = 0; i < current; ++i)
        if (canPlacedOn(i, position))
            return i;

    return 0;
}


/************************************************

 ************************************************/
void LxQtPanel::showConfigDialog()
{
    if (mConfigDialog.isNull())
        mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

    mConfigDialog->showConfigPanelPage();
    mConfigDialog->show();
    mConfigDialog->raise();
    mConfigDialog->activateWindow();
    WId wid = mConfigDialog->windowHandle()->winId();

    KWindowSystem::activateWindow(wid);
    KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());
}


/************************************************

 ************************************************/
void LxQtPanel::showAddPluginDialog()
{
    if (mConfigDialog.isNull())
        mConfigDialog = new ConfigPanelDialog(this, nullptr /*make it top level window*/);

    mConfigDialog->showConfigPluginsPage();
    mConfigDialog->show();
    mConfigDialog->raise();
    mConfigDialog->activateWindow();
    WId wid = mConfigDialog->windowHandle()->winId();

    KWindowSystem::activateWindow(wid);
    KWindowSystem::setOnDesktop(wid, KWindowSystem::currentDesktop());
}


/************************************************

 ************************************************/
void LxQtPanel::addPlugin(const LxQt::PluginInfo &desktopFile)
{
    QString settingsGroup = findNewPluginSettingsGroup(desktopFile.id());
    Plugin * plugin = loadPlugin(desktopFile, settingsGroup);
    if (0 != plugin)
    {
        mPluginsList << settingsGroup;

        realign();
        emit pluginAdded(LxQt::PluginData(plugin->desktopFile().id(), plugin, plugin->popupMenu()));
    }
    saveSettings(true);
}


/************************************************

 ************************************************/
void LxQtPanel::updateStyleSheet()
{
    QStringList sheet;
    sheet << QString("Plugin > * { qproperty-iconSize: %1px %1px; }").arg(mIconSize);
    sheet << QString("Plugin > * > * { qproperty-iconSize: %1px %1px; }").arg(mIconSize);

    if (mFontColor.isValid())
        sheet << QString("Plugin * { color: " + mFontColor.name() + "; }");

    QString object = LxQtPanelWidget->objectName();

    if (mBackgroundColor.isValid())
    {
        QString color = QString("%1, %2, %3, %4")
            .arg(mBackgroundColor.red())
            .arg(mBackgroundColor.green())
            .arg(mBackgroundColor.blue())
            .arg((float) mOpacity / 100);
        sheet << QString("LxQtPanel #BackgroundWidget { background-color: rgba(" + color + "); }");
    }

    if (QFileInfo(mBackgroundImage).exists())
        sheet << QString("LxQtPanel #BackgroundWidget { background-image: url('" + mBackgroundImage + "');}");

    setStyleSheet(sheet.join("\n"));
}



/************************************************

 ************************************************/
void LxQtPanel::setPanelSize(int value, bool save)
{
    if (mPanelSize != value)
    {
        mPanelSize = value;
        realign();

        if (save)
            saveSettings(true);
    }
}



/************************************************

 ************************************************/
void LxQtPanel::setIconSize(int value, bool save)
{
    if (mIconSize != value)
    {
        mIconSize = value;
        updateStyleSheet();
        mLayout->setLineSize(mIconSize);

        if (save)
            saveSettings(true);

        realign();
    }
}


/************************************************

 ************************************************/
void LxQtPanel::setLineCount(int value, bool save)
{
    if (mLineCount != value)
    {
        mLineCount = value;
        mLayout->setEnabled(false);
        mLayout->setLineCount(mLineCount);
        mLayout->setEnabled(true);

        if (save)
            saveSettings(true);

        realign();
    }
}


/************************************************

 ************************************************/
void LxQtPanel::setLength(int length, bool inPercents, bool save)
{
    if (mLength == length &&
            mLengthInPercents == inPercents)
        return;

    mLength = length;
    mLengthInPercents = inPercents;

    if (save)
        saveSettings(true);

    realign();
}


/************************************************

 ************************************************/
void LxQtPanel::setPosition(int screen, ILxQtPanel::Position position, bool save)
{
    if (mScreenNum == screen &&
            mPosition == position)
        return;

    mScreenNum = screen;
    mPosition = position;
    mLayout->setPosition(mPosition);

    if (save)
        saveSettings(true);

    // Qt 5 adds a new class QScreen and add API for setting the screen of a QWindow.
    // so we had better use it. However, without this, our program should still work
    // as long as XRandR is used. Since XRandR combined all screens into a large virtual desktop
    // every screen and their virtual siblings are actually on the same virtual desktop.
    // So things still work if we don't set the screen correctly, but this is not the case
    // for other backends, such as the upcoming wayland support. Hence it's better to set it.
    if(windowHandle())
    {
        // QScreen* newScreen = qApp->screens().at(screen);
        // QScreen* oldScreen = windowHandle()->screen();
        // const bool shouldRecreate = windowHandle()->handle() && !(oldScreen && oldScreen->virtualSiblings().contains(newScreen));
        // Q_ASSERT(shouldRecreate == false);

        // NOTE: When you move a window to another screen, Qt 5 might recreate the window as needed
        // But luckily, this never happen in XRandR, so Qt bug #40681 is not triggered here.
        // (The only exception is when the old screen is destroyed, Qt always re-create the window and
        // this corner case triggers #40681.)
        // When using other kind of multihead settings, such as Xinerama, this might be different and
        // unless Qt developers can fix their bug, we have no way to workaround that.
        windowHandle()->setScreen(qApp->screens().at(screen));
    }

    realign();
}

/************************************************
 *
 ************************************************/
void LxQtPanel::setAlignment(Alignment value, bool save)
{
    if (mAlignment == value)
        return;

    mAlignment = value;

    if (save)
        saveSettings(true);

    realign();
}

/************************************************
 *
 ************************************************/
void LxQtPanel::setFontColor(QColor color, bool save)
{
    mFontColor = color;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}

/************************************************

 ************************************************/
void LxQtPanel::setBackgroundColor(QColor color, bool save)
{
    mBackgroundColor = color;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}

/************************************************

 ************************************************/
void LxQtPanel::setBackgroundImage(QString path, bool save)
{
    mBackgroundImage = path;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}


/************************************************
 *
 ************************************************/
void LxQtPanel::setOpacity(int opacity, bool save)
{
    mOpacity = opacity;
    updateStyleSheet();

    if (save)
        saveSettings(true);
}


/************************************************

 ************************************************/
QRect LxQtPanel::globalGometry() const
{
    return QRect(mapToGlobal(QPoint(0, 0)), this->size());
}


/************************************************

 ************************************************/
bool LxQtPanel::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ContextMenu:
        showPopupMenu();
        break;

    case QEvent::LayoutRequest:
        emit realigned();
        break;

    case QEvent::WinIdChange:
    {
        // qDebug() << "WinIdChange" << hex << effectiveWinId();
        if(effectiveWinId() == 0)
            break;

        // Sometimes Qt needs to re-create the underlying window of the widget and
        // the winId() may be changed at runtime. So we need to reset all X11 properties
        // when this happens.
        qDebug() << "WinIdChange" << hex << effectiveWinId() << "handle" << windowHandle() << windowHandle()->screen();

        // Qt::WA_X11NetWmWindowTypeDock becomes ineffective in Qt 5
        // See QTBUG-39887: https://bugreports.qt-project.org/browse/QTBUG-39887
        // Let's do it manually
        NETWinInfo info(QX11Info::connection(), effectiveWinId(), QX11Info::appRootWindow(), NET::WMWindowType, 0);
        info.setWindowType(NET::Dock);

        updateWmStrut(); // reserve screen space for the panel
        KWindowSystem::setOnAllDesktops(effectiveWinId(), true);
        break;
    }
    case QEvent::DragEnter:
        event->ignore();
        //no break intentionally
    case QEvent::Enter:
        showPanel();
        break;

    case QEvent::Leave:
    case QEvent::DragLeave:
        hidePanel();
        break;

    default:
        break;
    }

    return QFrame::event(event);
}

/************************************************

 ************************************************/

void LxQtPanel::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);
    realign();
}


/************************************************

 ************************************************/
void LxQtPanel::showPopupMenu(Plugin *plugin)
{
    QList<QMenu*> pluginsMenus;
    PopupMenu menu(tr("Panel"));

    menu.setIcon(XdgIcon::fromTheme("configure-toolbars"));

    // Plugin Menu ..............................
    if (plugin)
    {
        QMenu *m = plugin->popupMenu();

        if (m)
        {
            menu.addTitle(plugin->windowTitle());
            menu.addActions(m->actions());
            pluginsMenus << m;
        }
    }

    // Panel menu ...............................

    menu.addTitle(QIcon(), tr("Panel"));

    menu.addAction(XdgIcon::fromTheme(QStringLiteral("configure")),
                   tr("Configure Panel..."),
                   this, SLOT(showConfigDialog())
                  );

    menu.addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Manage Panel Widgets..."),
                   this, SLOT(showAddPluginDialog())
                  );

    LxQtPanelApplication *a = reinterpret_cast<LxQtPanelApplication*>(qApp);
    menu.addAction(XdgIcon::fromTheme(QLatin1String("list-add")),
                   tr("Add Panel"),
                   a, SLOT(addNewPanel())
                  );

    if (a->count() > 1)
    {
        menu.addAction(XdgIcon::fromTheme(QStringLiteral("list-remove")),
                       tr("Remove Panel"),
                       this, SLOT(userRequestForDeletion())
                      );
    }

#ifdef DEBUG
    menu.addSeparator();
    menu.addAction("Exit (debug only)", qApp, SLOT(quit()));
#endif

    menu.exec(QCursor::pos());
    qDeleteAll(pluginsMenus);
}


/************************************************

 ************************************************/
Plugin *LxQtPanel::findPlugin(const ILxQtPanelPlugin *iPlugin) const
{
    foreach(Plugin *plugin, mPlugins)
    {
        if (plugin->iPlugin() == iPlugin)
            return plugin;
    }

    return 0;
}


/************************************************

 ************************************************/
QRect LxQtPanel::calculatePopupWindowPos(const ILxQtPanelPlugin *plugin, const QSize &windowSize) const
{
    Plugin *panelPlugin = findPlugin(plugin);
    if (!plugin)
        return QRect();

    int x=0, y=0;

    switch (position())
    {
    case ILxQtPanel::PositionTop:
        x = panelPlugin->mapToGlobal(QPoint(0, 0)).x();
        y = globalGometry().bottom();
        break;

    case ILxQtPanel::PositionBottom:
        x = panelPlugin->mapToGlobal(QPoint(0, 0)).x();
        y = globalGometry().top() - windowSize.height();
        break;

    case ILxQtPanel::PositionLeft:
        x = globalGometry().right();
        y = panelPlugin->mapToGlobal(QPoint(0, 0)).y();
        break;

    case ILxQtPanel::PositionRight:
        x = globalGometry().left() - windowSize.width();
        y = panelPlugin->mapToGlobal(QPoint(0, 0)).y();
        break;
    }

    QRect res(QPoint(x, y), windowSize);

    QRect screen = QApplication::desktop()->screenGeometry(this);
    // NOTE: We cannot use AvailableGeometry() which returns the work area here because when in a
    // multihead setup with different resolutions. In this case, the size of the work area is limited
    // by the smallest monitor and may be much smaller than the current screen and we will place the
    // menu at the wrong place. This is very bad for UX. So let's use the full size of the screen.
    if (res.right() > screen.right())
        res.moveRight(screen.right());

    if (res.bottom() > screen.bottom())
        res.moveBottom(screen.bottom());

    if (res.left() < screen.left())
        res.moveLeft(screen.left());

    if (res.top() < screen.top())
        res.moveTop(screen.top());

    return res;
}


/************************************************

 ************************************************/
QString LxQtPanel::qssPosition() const
{
    return positionToStr(position());
}


/************************************************

 ************************************************/
QString LxQtPanel::findNewPluginSettingsGroup(const QString &pluginType) const
{
    QStringList groups = mSettings->childGroups();
    groups.sort();

    // Generate new section name
    for (int i = 2; true; ++i)
        if (!groups.contains(QString("%1%2").arg(pluginType).arg(i)))
            return QString("%1%2").arg(pluginType).arg(i);
}


/************************************************

 ************************************************/
void LxQtPanel::removePlugin()
{
    Plugin *plugin = qobject_cast<Plugin*>(sender());
    if (plugin)
    {
        mSettings->remove(plugin->settingsGroup());
        mPluginsList.removeAll(plugin->settingsGroup());
        mPlugins.removeAll(plugin);
    }

    saveSettings();
    emit pluginRemoved(LxQt::PluginData(plugin->desktopFile().id(), plugin, nullptr/*don't want any menu*/));
}


/************************************************

 ************************************************/
void LxQtPanel::pluginMoved(Plugin const * plug)
{
    //get new position of the moved plugin
    QString plug_is_after, last_name;
    for (int i=0; i<mLayout->count(); ++i)
    {
        Plugin *plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
        {
            if (plug == plugin)
                plug_is_after = last_name; //is after previous name (or empty as first)
            last_name = plugin->settingsGroup();
        }
    }
    //merge list of plugins (try to preserve original position)
    mPluginsList.removeAll(plug->settingsGroup());
    mPluginsList.insert(mPluginsList.indexOf(plug_is_after)/*-1 if not found*/ + 1, plug->settingsGroup());
    saveSettings();
}


/************************************************

 ************************************************/
void LxQtPanel::userRequestForDeletion()
{
    mSettings->beginGroup(mConfigGroup);
    QStringList plugins = mSettings->value("plugins").toStringList();
    mSettings->endGroup();

    Q_FOREACH(QString i, plugins)
        if (!i.isEmpty())
            mSettings->remove(i);

    mSettings->remove(mConfigGroup);

    emit deletedByUser(this);
}

void LxQtPanel::showPanel()
{
    if (mHidable && mHidden)
    {
        mHidden = false;
        mHideTimer.stop();
        realign();
    }
}

void LxQtPanel::hidePanel()
{
    if (mHidable && !mHidden && !geometry().contains(QCursor::pos()))
    {
        mHideTimer.start();
    }
}

void LxQtPanel::hidePanelWork()
{
    mHidden = true;
    realign();
}

void LxQtPanel::setHidable(bool hidable, bool save)
{
    if (mHidable == hidable)
        return;

    mHidable = hidable;
    mHidden = mHidable;

    if (save)
        saveSettings(true);

    realign();
}
