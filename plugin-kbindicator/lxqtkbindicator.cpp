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

#include "lxqtkbindicator.h"
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QTimer>

LxQtKbIndicator::LxQtKbIndicator(const ILxQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILxQtPanelPlugin(startupInfo),
    mContent(new QWidget()),
    mLayout(settings())
{
    modifierInfo = new KModifierKeyInfo(this);
    connect(modifierInfo, SIGNAL(keyLocked(Qt::Key, bool)), this, SLOT(modifierStateChanged(Qt::Key, bool)));

    QHBoxLayout *layout = new QHBoxLayout(mContent);
    mContent->setLayout(layout);

    mCapsLock = new QLabel("C");
    mCapsLock->setObjectName("CapsLockLabel");
    mCapsLock->setAlignment(Qt::AlignCenter);
    mCapsLock->installEventFilter(this);
    mContent->layout()->addWidget(mCapsLock);

    mNumLock = new QLabel("N");
    mNumLock->setObjectName("NumLockLabel");
    mNumLock->setAlignment(Qt::AlignCenter);
    mNumLock->installEventFilter(this);
    mContent->layout()->addWidget(mNumLock);

    mScrollLock = new QLabel("S");
    mScrollLock->setObjectName("ScrollLockLabel");
    mScrollLock->setAlignment(Qt::AlignCenter);
    mScrollLock->installEventFilter(this);
    mContent->layout()->addWidget(mScrollLock);

    if (mLayout.enabled()){
        mContent->layout()->addWidget(mLayout.widget());
        mLayout.widget()->installEventFilter(this);
    }

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

LxQtKbIndicator::~LxQtKbIndicator()
{
    delete mContent;
}

void LxQtKbIndicator::delayedInit()
{
    settingsChanged();
    realign();
}

QWidget *LxQtKbIndicator::widget()
{
    return mContent;
}

void LxQtKbIndicator::settingsChanged()
{
    mShowCapsLock = settings()->value("show_caps_lock", true).toBool();
    mShowNumLock = settings()->value("show_num_lock", true).toBool();
    mShowScrollLock = settings()->value("show_scroll_lock", true).toBool();

    mCapsLock->setVisible(mShowCapsLock);
    mNumLock->setVisible(mShowNumLock);
    mScrollLock->setVisible(mShowScrollLock);

    mCapsLock->setEnabled(modifierInfo->isKeyLocked(Qt::Key_CapsLock));
    mNumLock->setEnabled(modifierInfo->isKeyLocked(Qt::Key_NumLock));
    mScrollLock->setEnabled(modifierInfo->isKeyLocked(Qt::Key_ScrollLock));

    if(mLayout.enabled()){
        mLayout.widget()->setVisible(settings()->value("show_layout", true).toBool());
    }
}

QDialog *LxQtKbIndicator::configureDialog()
{
    return new LxQtKbIndicatorConfiguration(settings());
}

void LxQtKbIndicator::realign()
{
    if (panel()->isHorizontal())
        mContent->setMinimumSize(0, panel()->iconSize());
    else
        mContent->setMinimumSize(panel()->iconSize(), 0);
}

void LxQtKbIndicator::modifierStateChanged(Qt::Key key, bool active)
{
    switch (key)
    {
        case Qt::Key_CapsLock:
            mCapsLock->setEnabled(active);
            break;

        case Qt::Key_NumLock:
            mNumLock->setEnabled(active);
            break;

        case Qt::Key_ScrollLock:
            mScrollLock->setEnabled(active);
            break;

        default:
            break;
    }
}

bool LxQtKbIndicator::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::QEvent::MouseButtonRelease)
    {
        if (object == mCapsLock)
            modifierInfo->setKeyLocked(Qt::Key_CapsLock, !modifierInfo->isKeyLocked(Qt::Key_CapsLock));
        else if (object == mNumLock)
            modifierInfo->setKeyLocked(Qt::Key_NumLock, !modifierInfo->isKeyLocked(Qt::Key_NumLock));
        else if (object == mScrollLock)
            modifierInfo->setKeyLocked(Qt::Key_ScrollLock, !modifierInfo->isKeyLocked(Qt::Key_ScrollLock));
        else if(mLayout.enabled() && object == mLayout.widget()){
            mLayout.switchNext();
        }

        return true;
    }

    return QObject::eventFilter(object, event);
}
