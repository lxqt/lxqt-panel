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
#include <LXQt/AddPluginDialog>
#include <LXQt/Settings>
#include <LXQt/PluginInfo>

#include <LXQt/XfitMan>

#include <QtCore/QDebug>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMenu>
#include <XdgIcon>

#include <XdgDirs>

// Config keys and groups
#define CFG_KEY_SCREENNUM   "desktop"
#define CFG_KEY_POSITION    "position"
#define CFG_KEY_ICONSIZE    "iconSize"
#define CFG_KEY_LINECNT     "lineCount"
#define CFG_KEY_LENGTH      "width"
#define CFG_KEY_PERCENT     "width-percent"
#define CFG_KEY_ALIGNMENT   "alignment"
#define CFG_KEY_PLUGINS     "plugins"

#define CFG_KEY_AUTOHIDE            "autohideTb"
#define CFG_KEY_AUTOHIDEDURATION    "autohideDuration"
#define AUTOHIDETB_SPACE 1 // unit pixels


using namespace LxQt;

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
        case LxQtPanel::PositionTop:    return QString("Top");
        case LxQtPanel::PositionLeft:   return QString("Left");
        case LxQtPanel::PositionRight:  return QString("Right");
        case LxQtPanel::PositionBottom: return QString("Bottom");
    }

    return QString();
}


/************************************************

 ************************************************/
LxQtPanel::LxQtPanel(const QString &configGroup, QWidget *parent) :
    QFrame(parent),
    mConfigGroup(configGroup),
    mIconSize(0),
    mLineCount(0),
    mChilds(0),
    mAutoHideActive(false),
    mAutoHideLock(false),
    mInitialized(0),
    mAutoHidePermanentLock(false),
    mAnimationOffset(100)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("LxQt Panel");
    setObjectName(QString("LxQtPanel %1").arg(configGroup));

    animationPanel = new VariantAnimation(parent, &mAnimationOffset);

    LxQtPanelWidget = new QFrame(this);
    LxQtPanelWidget->setObjectName("BackgroundWidget");
    QGridLayout* lav = new QGridLayout();
    lav->setContentsMargins(QMargins(0,0,0,0));
    setLayout(lav);
    this->layout()->addWidget(LxQtPanelWidget);

    mLayout = new LxQtPanelLayout(LxQtPanelWidget);
    connect(mLayout, SIGNAL(pluginMoved()), this, SLOT(pluginMoved()));
    LxQtPanelWidget->setLayout(mLayout);
    mLayout->setLineCount(mLineCount);

    mDelaySave.setSingleShot(true);
    mDelaySave.setInterval(SETTINGS_SAVE_DELAY);
    connect(&mDelaySave, SIGNAL(timeout()), this, SLOT(saveSettings()));

    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(screensChangeds()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), this, SLOT(realign()));
    connect(QApplication::desktop(), SIGNAL(screenCountChanged(int)), this, SLOT(screensChangeds()));
    connect(LxQt::Settings::globalSettings(), SIGNAL(settingsChanged()), this, SLOT(update()));
    connect(lxqtApp, SIGNAL(themeChanged()), this, SLOT(realign()));

    connect(mLayout,SIGNAL(pluginMoving()), this, SLOT(autohidePermanentLock()));
    connect(mLayout,SIGNAL(pluginMoved()), this, SLOT(autohidePermanentUnlock()));

    LxQtPanelApplication *app = reinterpret_cast<LxQtPanelApplication*>(qApp);
    mSettings = app->settings();
    readSettings();
    loadPlugins();

    // timer for not remembering opened windows from panel
    mAutoHideTimerLock = 0;
    mAutohideTimer.setSingleShot(true);
    mAutohideTimer.setInterval(600);
    connect(&mAutohideTimer, SIGNAL(timeout()), this, SLOT(autohideRemoveTimerLock()));

    // panel animation
    animationPanel->setStartValue(0);
    animationPanel->setEndValue(100);
    animationPanel->setEasingCurve(QEasingCurve::OutExpo); //QEasingCurve::OutSine // QEasingCurve::OutInBack // QEasingCurve::OutCirc
    animationPanel->setDuration(mAutoHideDuration);
    connect (animationPanel,SIGNAL(valueChanged()), this, SLOT(updateGeometry()));

    // startup apply show or hide
    autohideActive(mAutoHideTb);

    show();
}


/************************************************

 ************************************************/
void LxQtPanel::readSettings()
{
    // Read settings ......................................
    mSettings->beginGroup(mConfigGroup);

    // By default we are using size & count from theme.
    setIconSize(mSettings->value(CFG_KEY_ICONSIZE, PANEL_DEFAULT_ICON_SIZE).toInt());
    setLineCount(mSettings->value(CFG_KEY_LINECNT, PANEL_DEFAULT_LINE_COUNT).toInt());

    setLength(mSettings->value(CFG_KEY_LENGTH, 100).toInt(),
              mSettings->value(CFG_KEY_PERCENT, true).toBool());

    setPosition(mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt(),
                strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), PositionBottom));

    setAlignment(LxQtPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, mAlignment).toInt()));

    setAutohide(mSettings->value(CFG_KEY_AUTOHIDE,PANEL_DEFAULT_AUTOHIDE).toBool());
    setAutohideDuration(mSettings->value(CFG_KEY_AUTOHIDEDURATION,PANEL_DEFAULT_AUTOHIDEDURATION).toInt());

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

    QStringList pluginsList;

    mSettings->beginGroup(mConfigGroup);

    foreach (const Plugin *plugin, mPlugins)
    {
        pluginsList << plugin->settingsGroup();
    }

    if (pluginsList.isEmpty())
    {
        mSettings->setValue(CFG_KEY_PLUGINS, "");
    }
    else
    {
        mSettings->setValue(CFG_KEY_PLUGINS, pluginsList);
    }

    mSettings->setValue(CFG_KEY_ICONSIZE, mIconSize);
    mSettings->setValue(CFG_KEY_LINECNT,  mLineCount);

    mSettings->setValue(CFG_KEY_LENGTH,   mLength);
    mSettings->setValue(CFG_KEY_PERCENT,  mLengthInPercents);

    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->setValue(CFG_KEY_POSITION, positionToStr(mPosition));

    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);

    mSettings->setValue(CFG_KEY_AUTOHIDE, mAutoHideTb);
    mSettings->setValue(CFG_KEY_AUTOHIDEDURATION, mAutoHideDuration);

    mSettings->endGroup();
}


/************************************************

 ************************************************/
void LxQtPanel::screensChangeds()
{
    if (! canPlacedOn(mScreenNum, mPosition))
        setPosition(findAvailableScreen(mPosition), mPosition);
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
    xfitMan().moveWindowToDesktop(this->effectiveWinId(), -1);
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
    QStringList sections = mSettings->value(CFG_KEY_PLUGINS).toStringList();
    mSettings->endGroup();

    foreach (QString sect, sections)
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
        connect(plugin,SIGNAL(autoHidePermanentLock()), this, SLOT(autohidePermanentLock()));
        connect(plugin,SIGNAL(autoHidePermanentUnlock()), this, SLOT(autohidePermanentUnlock()));

        mLayout->addWidget(plugin);
        return plugin;
    }

    delete plugin;
    return 0;
}


/************************************************

 ************************************************/
void VariantAnimation::updateCurrentValue(const QVariant &value)
{
    if (*currentValue != value.toInt())
    {
        *currentValue = value.toInt();
        emit valueChanged();
    }
}


/************************************************

 ************************************************/
void LxQtPanel::updateGeometry()
{
    QSize size = sizeHint();

    int defaultsize;

    if (isHorizontal())
        defaultsize = size.height();
    else
        defaultsize = size.width();

    if (mAutoHideTb)
        mOffset = mAnimationOffset;
    else
        mOffset = 100;

    const QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);

    // percent to pixels
    int mOffsetPixels = (defaultsize * (100 - mOffset)/100);
    if (mOffsetPixels >= defaultsize)
        mOffsetPixels = defaultsize - 1;

    if (isHorizontal())
    {
        // Horiz ......................
        switch (mAlignment)
        {
        case LxQtPanel::AlignmentLeft:
            rect.moveLeft(screen.left());
            break;

        case LxQtPanel::AlignmentCenter:
            rect.moveCenter(screen.center());
            break;

        case LxQtPanel::AlignmentRight:
            rect.moveRight(screen.right());
            break;
        }

        // Vert .......................
        if (mPosition == ILxQtPanel::PositionTop)
            rect.moveTop(screen.top() - mOffsetPixels);
        else
            rect.moveBottom(screen.bottom() + mOffsetPixels);
    }
    else
    {
        // Vert .......................
        switch (mAlignment)
        {
        case LxQtPanel::AlignmentLeft:
            rect.moveTop(screen.top());
            break;

        case LxQtPanel::AlignmentCenter:
            rect.moveCenter(screen.center());
            break;

        case LxQtPanel::AlignmentRight:
            rect.moveBottom(screen.bottom());
            break;
        }

        // Horiz ......................
        if (mPosition == ILxQtPanel::PositionLeft)
            rect.moveLeft(screen.left() - mOffsetPixels);
        else
            rect.moveRight(screen.right() + mOffsetPixels);
    }

    // show changes
    setGeometry(rect);
    setFixedSize(rect.size());
}


/************************************************

 ************************************************/
void LxQtPanel::realign()
{
    if (!isVisible())
        return;
#if 0
    qDebug() << "** Realign *********************";
    qDebug() << "IconSize:      " << mIconSize;
    qDebug() << "LineCount:     " << mLineCount;
    qDebug() << "Length:        " << mLength << (mLengthInPercents ? "%" : "px");
    qDebug() << "Alignment:     " << (mAlignment == 0 ? "center" : (mAlignment < 0 ? "left" : "right"));
    qDebug() << "Position:      " << positionToStr(mPosition) << "on" << mScreenNum;
    qDebug() << "Plugins count: " << mPlugins.count();
#endif

    const QRect screen = QApplication::desktop()->screenGeometry(mScreenNum);

    QSize size = sizeHint();

    if (isHorizontal())
    {
        // Horiz panel ***************************

        // Size .......................
        rect.setHeight(qMax(PANEL_MINIMUM_SIZE, size.height()));

        if (mLengthInPercents)
            rect.setWidth(screen.width() * mLength / 100.0);
        else
        {
            if (mLength <= 0)
                rect.setWidth(screen.width() + mLength);
            else
                rect.setWidth(mLength);
        }

        rect.setWidth(qMax(rect.size().width(), mLayout->minimumSize().width()));
    }
    else
    {
        // Vert panel ***************************

        // Size .......................
        rect.setWidth(qMax(PANEL_MINIMUM_SIZE, size.width()));

        if (mLengthInPercents)
            rect.setHeight(screen.height() * mLength / 100.0);
        else
        {
            if (mLength <= 0)
                rect.setHeight(screen.height() + mLength);
            else
                rect.setHeight(mLength);
        }

        rect.setHeight(qMax(rect.size().height(), mLayout->minimumSize().height()));
    }

    // autohide
    if (mAutoHideTb)
    {
        // panel should hide or not?
        if (mAutoHideActive && !mAutoHideLock  && !mChilds  && !mAutoHidePermanentLock)
        {
            // switch animation direction if necessary
            if (animationPanel->direction() == QAbstractAnimation::Forward)
            {
                animationPanel->setDirection(QAbstractAnimation::Backward);
                if (animationPanel->state() == QAbstractAnimation::Stopped)
                    animationPanel->start();
            }
        }
        else
        {
            // switch animation direction if necessary
            if (animationPanel->direction() == QAbstractAnimation::Backward)
            {
                animationPanel->setDirection(QAbstractAnimation::Forward);
                if (animationPanel->state() == QAbstractAnimation::Stopped)
                    animationPanel->start();
            }
        }
    }

    updateGeometry();

    // Reserve our space on the screen ..........
    XfitMan xf = xfitMan();
    Window wid = effectiveWinId();

    // ... depeding on autohide taskbar on or off
    if (mAutoHideTb)
    {
        switch (mPosition)
        {
            case LxQtPanel::PositionTop:
                xf.setStrut(wid, 0, 0, AUTOHIDETB_SPACE, 0,
                   /* Left   */  0, 0,
                   /* Right  */  0, 0,
                   /* Top    */  rect.left(), rect.right(),
                   /* Bottom */  0, 0
                             );
            break;

            case LxQtPanel::PositionBottom:

            xf.setStrut(wid, 0, 0, 0, AUTOHIDETB_SPACE,
                   /* Left   */  0, 0,
                   /* Right  */  0, 0,
                   /* Top    */  0, 0,
                   /* Bottom */  rect.left(), rect.right()
                             );
                break;

            case LxQtPanel::PositionLeft:
                xf.setStrut(wid, AUTOHIDETB_SPACE, 0, 0, 0,
                   /* Left   */  rect.top(), rect.bottom(),
                   /* Right  */  0, 0,
                   /* Top    */  0, 0,
                   /* Bottom */  0, 0
                             );

                break;

            case LxQtPanel::PositionRight:
                xf.setStrut(wid, 0, AUTOHIDETB_SPACE, 0, 0,
                   /* Left   */  0, 0,
                   /* Right  */  rect.top(), rect.bottom(),
                   /* Top    */  0, 0,
                   /* Bottom */  0, 0
                             );
                break;
        }
    }
    else
    {
        switch (mPosition)
        {
            case LxQtPanel::PositionTop:
                xf.setStrut(wid, 0, 0, height(), 0,
                   /* Left   */  0, 0,
                   /* Right  */  0, 0,
                   /* Top    */  rect.left(), rect.right(),
                   /* Bottom */  0, 0
                             );
            break;

            case LxQtPanel::PositionBottom:
               xf.setStrut(wid, 0, 0, 0, (screen.height() - rect.y()), //problem with two monitors on smaller monitor?
                   /* Left   */  0, 0,
                   /* Right  */  0, 0,
                   /* Top    */  0, 0,
                   /* Bottom */  rect.left(), rect.right()
                             );
                break;

            case LxQtPanel::PositionLeft:
                xf.setStrut(wid, width(), 0, 0, 0,
                   /* Left   */  rect.top(), rect.bottom(),
                   /* Right  */  0, 0,
                   /* Top    */  0, 0,
                   /* Bottom */  0, 0
                             );

                break;

            case LxQtPanel::PositionRight:
                xf.setStrut(wid, 0, width(), 0, 0,
                   /* Left   */  0, 0,
                   /* Right  */  rect.top(), rect.bottom(),
                   /* Top    */  0, 0,
                   /* Bottom */  0, 0
                             );
                break;
        }
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
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).bottom() < dw->screenGeometry(screenNum).top())
                    return false;
            }
            return true;

        case LxQtPanel::PositionBottom:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).top() > dw->screenGeometry(screenNum).bottom())
                    return false;
            }
            return true;

        case LxQtPanel::PositionLeft:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).right() < dw->screenGeometry(screenNum).left())
                    return false;
            }
            return true;

        case LxQtPanel::PositionRight:
            for (int i=0; i < dw->screenCount(); ++i)
            {
                if (dw->screenGeometry(i).left() > dw->screenGeometry(screenNum).right())
                    return false;
            }
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
    {
        if (canPlacedOn(i, position))
            return i;
    }

    for (int i = 0; i < current; ++i)
    {
        if (canPlacedOn(i, position))
            return i;
    }

    return 0;
}


/************************************************

 ************************************************/
void LxQtPanel::showConfigDialog()
{
    ConfigPanelDialog::exec(this);
}


/************************************************

 ************************************************/
void LxQtPanel::showAddPluginDialog()
{
    AddPluginDialog* dialog = findChild<AddPluginDialog*>();

    if (!dialog)
    {
        dialog = new AddPluginDialog(pluginDesktopDirs(), "LxQtPanel/Plugin", "*", this);
        dialog->setWindowTitle(tr("Add Panel Widgets"));
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialog, SIGNAL(pluginSelected(const LxQt::PluginInfo&)), this, SLOT(addPlugin(const LxQt::PluginInfo&)));
        connect(dialog,SIGNAL(destroyed()), this, SLOT(autohidePermanentUnlock()));

        emit autohidePermanentLock();
    }
    
    LxQt::PluginInfoList pluginsInUse;
    foreach (Plugin *i, mPlugins)
        pluginsInUse << i->desktopFile();
    dialog->setPluginsInUse(pluginsInUse);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    xfitMan().raiseWindow(dialog->effectiveWinId());
    xfitMan().moveWindowToDesktop(dialog->effectiveWinId(), qMax(xfitMan().getActiveDesktop(), 0));

    dialog->show();
}


/************************************************

 ************************************************/
void LxQtPanel::addPlugin(const LxQt::PluginInfo &desktopFile)
{
    QString settingsGroup = findNewPluginSettingsGroup(desktopFile.id());
    loadPlugin(desktopFile, settingsGroup);
    saveSettings(true);

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::updateStyleSheet()
{
    QStringList sheet;
    sheet << QString("Plugin > * { qproperty-iconSize: %1px %1px; }").arg(mIconSize);
    sheet << QString("Plugin > * > * { qproperty-iconSize: %1px %1px; }").arg(mIconSize);

    setStyleSheet(sheet.join("\n"));
}



/************************************************

 ************************************************/
void LxQtPanel::setIconSize(int value)
{
    if (mIconSize != value)
    {
        mIconSize = value;
        updateStyleSheet();
        mLayout->setLineSize(mIconSize);
        saveSettings(true);

        realign();
        emit realigned();
    }
}


/************************************************
 *
 * *********************************************/
void LxQtPanel::setAutohide(bool value)
{
    if (mAutoHideTb != value)
    {
        mAutoHideTb = value;
        updateStyleSheet();
        saveSettings(true);

        if (value)
        {
            mAutoHideActive = true;
            mAutoHideLock = false;
            mAutoHidePermanentLock = false;
            mAutoHideTimerLock = 0;
            mMapped.clear();
        }

        realign();
        emit realigned();
    }
}


/************************************************
 *
 * *********************************************/
void LxQtPanel::setAutohideDuration(int value)
{
    if (mAutoHideDuration != value)
    {
        mAutoHideDuration = value;
        updateStyleSheet();
        saveSettings(true);

        animationPanel->setDuration(mAutoHideDuration);

        realign();
        emit realigned();
    }
}


/************************************************

 ************************************************/
void LxQtPanel::setLineCount(int value)
{
    if (mLineCount != value)
    {
        mLineCount = value;
        mLayout->setEnabled(false);
        mLayout->setLineCount(mLineCount);        
        mLayout->setEnabled(true);        
        saveSettings(true);

        realign();
        emit realigned();
    }
}


/************************************************

 ************************************************/
void LxQtPanel::setLength(int length, bool inPercents)
{
    if (mLength == length &&
        mLengthInPercents == inPercents)
        return;

    mLength = length;
    mLengthInPercents = inPercents;    
    saveSettings(true);

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::setPosition(int screen, ILxQtPanel::Position position)
{
    if (mScreenNum == screen &&
        mPosition == position)
        return;

    mScreenNum = screen;
    mPosition = position;
    mLayout->setPosition(mPosition);
    saveSettings(true);    

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::setAlignment(LxQtPanel::Alignment value)
{
    if (mAlignment == value)
        return;

    mAlignment = value;    
    saveSettings(true);

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::autohideRemoveTimerLock()
{
    mAutoHideTimerLock = 0;
}


/************************************************

 ************************************************/
void LxQtPanel::autohideActive(bool value)
{
    if (!mAutoHideTb && !mAutoHideActive)
        return;

    mAutoHideActive = value;
}


/************************************************
Will permanently lock the autohide function. until autohidePermanentUnlock is called.
Increments mAutoHidePermantenLock. Only if this var is 0 the panel can hide.
 ************************************************/
void LxQtPanel::autohidePermanentLock()
{
    mAutoHidePermanentLock += 1;

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::autohideLock()
{
    mAutoHideLock = true;

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::autohidePermanentUnlock()
{
    if (mAutoHidePermanentLock)
        mAutoHidePermanentLock -= 1;

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::autohideUnlock()
{
    mAutoHideLock = false;

    realign();
    emit realigned();
}


/************************************************
    if (!mAutoHideTb)
        return;
 ************************************************/
void LxQtPanel::x11EventFilter(XEvent* event, AutohideMsg type, long int win)
{
    //avoid adding windows to mapped list for a certain time
    if (type == SysTrayConfigure)
    {
        if (mAutoHideTb && !mAutoHideActive)
        {
            mAutoHideTimerLock = 1;
            mAutohideTimer.start();
        }
    }

    // remove window from mapped list
    if (mAutoHideTb && type == RemoveWindow)
    {
        mMapped -= win;

        // is there a mapped window?
        if (!mMapped.count())
            autohideUnlock();
    }

    // add window to mapped list if panel is active (e.g. mouseover)
    if (mAutoHideTb && !mAutoHideActive && !mAutoHideTimerLock && type == SaveWindow)
    {
        mMapped << win;
        autohideLock();
    }

    QList<Plugin*>::iterator i;
    for (i = mPlugins.begin(); i != mPlugins.end(); ++i)
        (*i)->x11EventFilter(event);
}


/************************************************

 ************************************************/
QRect LxQtPanel::globalGometry() const
{
    return QRect(mapToGlobal(QPoint(0, 0)), this->size());
}


/************************************************
when a child is added from the panel it shouldn't hide
 ************************************************/
void LxQtPanel::childEvent(QChildEvent *event)
{

    if (!mAutoHideTb)
        return;

    switch (event->type())
    {
        case QEvent::ChildAdded:
            if (mInitialized)
            {
                mChilds += 1;
                autohideLock();
            }
        break;

        case QEvent::ChildRemoved:
            if (mChilds < 2)
                mChilds = 0;
            else
                mChilds -= 1;

            if (!mChilds)
            autohideUnlock();
        break;

        default:
            break;
    }
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
        realign();
        emit realigned();
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
    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::enterEvent(QEvent *event)
{
    mInitialized = 1;
    autohideActive(false);

    realign();
    emit realigned();
}


/************************************************

 ************************************************/
void LxQtPanel::leaveEvent(QEvent *event)
{
    autohideActive(true);

    realign();
    emit realigned();
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
            menu.addTitle(plugin->windowTitle().replace("&", "&&"));

            menu.addActions(m->actions());
            pluginsMenus << m;
        }
    }

    // Panel menu ...............................

    menu.addTitle(QIcon(), tr("Panel"));

    menu.addAction(tr("Configure Panel..."),
                   this, SLOT(showConfigDialog())
                  );

    menu.addAction(XdgIcon::fromTheme("preferences-plugin"),
                   tr("Add Panel Widgets..."),
                   this, SLOT(showAddPluginDialog())
                  );

    LxQtPanelApplication *a = reinterpret_cast<LxQtPanelApplication*>(qApp);
    menu.addAction(tr("Add Panel"),
                   a, SLOT(addNewPanel())
                  );

    if (a->count() > 1)
    {
        menu.addAction(tr("Remove Panel"),
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

    QRect screen = QApplication::desktop()->availableGeometry(this);

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

    // Generate new section name ................
    for (int i=2; true; ++i)
    {
        if (!groups.contains(QString("%1%2").arg(pluginType).arg(i)))
        {
            return QString("%1%2").arg(pluginType).arg(i);
        }
    }
}


/************************************************

 ************************************************/
void LxQtPanel::removePlugin()
{
    Plugin *plugin = qobject_cast<Plugin*>(sender());
    if (plugin)
    {
        mPlugins.removeAll(plugin);
    }
    saveSettings();
}


/************************************************

 ************************************************/
void LxQtPanel::pluginMoved()
{
    mPlugins.clear();
    for (int i=0; i<mLayout->count(); ++i)
    {
        Plugin *plugin = qobject_cast<Plugin*>(mLayout->itemAt(i)->widget());
        if (plugin)
            mPlugins << plugin;
    }
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
        mSettings->remove(i);
    
    mSettings->remove(mConfigGroup);

    emit deletedByUser(this);
}
