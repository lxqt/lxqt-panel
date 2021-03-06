/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2021 LXQt team
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

#include "lxqtcustom.h"
#include "custombutton.h"
#include "lxqtcustomconfiguration.h"

#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <XdgIcon>
#include <LXQt/Globals>
#include <QDebug>

LXQtCustom::LXQtCustom(const ILXQtPanelPluginStartupInfo &startupInfo):
        QObject(),
        ILXQtPanelPlugin(startupInfo),
        mProcess(new QProcess(this)),
        mTimer(new QTimer(this)),
        mFirstRun(true),
        mOutput(QString()),
        mAutoRotate(true)
{
    mMainWidget = new QWidget;
    mMainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mContent = new CustomButton(this);
    mContent->setObjectName(QLatin1String("CustomContent"));

    mFont = mContent->font().toString();

    mTimer->setSingleShot(true);

    connect(mContent, &CustomButton::clicked, this, &LXQtCustom::handleClick);
    connect(mContent, &CustomButton::wheelScrolled, this, &LXQtCustom::wheelScrolled);
    connect(mTimer, &QTimer::timeout, this, &LXQtCustom::runCommand);
    connect(mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &LXQtCustom::handleFinished);

    settingsChanged();
}

LXQtCustom::~LXQtCustom()
{
    delete mContent;
}

QWidget *LXQtCustom::widget()
{
    return mContent;
}

void LXQtCustom::realign()
{
    mContent->setAutoRotation(mAutoRotate);
}

QDialog *LXQtCustom::configureDialog()
{
    if (!mConfigDialog) {
        mConfigDialog = new LXQtCustomConfiguration(settings());
        mConfigDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    }
    return mConfigDialog;
}

void LXQtCustom::settingsChanged()
{
    bool shouldRun = false;

    bool oldAutoRotate = mAutoRotate;
    QString oldFont = mFont;
    QString oldCommand = mCommand;
    bool oldRunWithBash = mRunWithBash;
    bool oldRepeat = mRepeat;
    int oldRepeatTimer = mRepeatTimer;
    QString oldIcon = mIcon;
    QString oldText = mText;
    int oldMaxWidth = mMaxWidth;

    mAutoRotate = settings()->value(QStringLiteral("autoRotate"), true).toBool();
    mFont = settings()->value(QStringLiteral("font"), mContent->font().toString()).toString();
    mCommand = settings()->value(QStringLiteral("command"), QString()).toString();
    mRunWithBash = settings()->value(QStringLiteral("runWithBash"), true).toBool();
    mRepeat = settings()->value(QStringLiteral("repeat"), true).toBool();
    mRepeatTimer = settings()->value(QStringLiteral("repeatTimer"), 1000).toInt();
    mIcon = settings()->value(QStringLiteral("icon"), QString()).toString();
    mText = settings()->value(QStringLiteral("text"), QStringLiteral("%1")).toString();
    mMaxWidth = settings()->value(QStringLiteral("maxWidth"), 200).toInt();
    mClick = settings()->value(QStringLiteral("click"), QString()).toString();
    mWheelUp = settings()->value(QStringLiteral("wheelUp"), QString()).toString();
    mWheelDown = settings()->value(QStringLiteral("wheelDown"), QString()).toString();

    if (oldFont != mFont) {
        QFont newFont;
        newFont.fromString(mFont);
        QTimer::singleShot(100, [=] {
            mContent->setFont(newFont);
        });
    }
    if (oldCommand != mCommand || oldRunWithBash != mRunWithBash || oldRepeat != mRepeat)
        shouldRun = true;

    if (oldRepeatTimer != mRepeatTimer)
        mTimer->setInterval(mRepeatTimer);

    if (oldIcon != mIcon || oldText != mText)
        mContent->setIcon(XdgIcon::fromTheme(mIcon, QIcon(mIcon)));

    if (oldMaxWidth != mMaxWidth)
        mContent->setMaxWidth(mMaxWidth);

    if (oldAutoRotate != mAutoRotate)
        mContent->setAutoRotation(mAutoRotate);

    if (shouldRun || mFirstRun) {
        mFirstRun = false;
        runCommand();
    }
}

void LXQtCustom::handleClick()
{
    if (!mClick.isEmpty())
        runDetached(mClick);
}

void LXQtCustom::handleFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{

    if (exitCode == 0) {
        mOutput = QString::fromUtf8(mProcess->readAllStandardOutput());
        if (mOutput.endsWith(QStringLiteral("\n")))
            mOutput.chop(1);
    }
    else
        mOutput = tr("Error");

    updateButton();
    if (mRepeat)
        mTimer->start();
}

void LXQtCustom::updateButton(){
    QString newText = mText;
    if (newText.contains(QStringLiteral("%1")))
        newText = newText.arg(mOutput);

    if(mContent->icon().isNull())
         mContent->setToolButtonStyle(Qt::ToolButtonTextOnly);
    else
         mContent->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mContent->setText(newText);
    mContent->updateWidth();
}

void LXQtCustom::wheelScrolled(int yDelta)
{
    if (yDelta > 0 && !mWheelUp.isEmpty())
        runDetached(mWheelUp);
    else if (yDelta < 0 && !mWheelDown.isEmpty())
        runDetached(mWheelDown);
}

void LXQtCustom::runCommand()
{
    if (mCommand.isEmpty()) {
        return;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
    QStringList args;
    if (mRunWithBash)
        args << QStringLiteral("bash") << QStringLiteral("-c") << mCommand;
    else
        args = QProcess::splitCommand(mCommand);

    mProcess->start(args.takeFirst(), args);
#else
    QString exec;
    if (mRunWithBash)
        exec = QString(QStringLiteral("bash -c \"%1\"")).arg(mCommand);
    else
        exec = mCommand;

    mProcess->start(exec);
#endif
}

void LXQtCustom::runDetached(QString command)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
    QStringList args = QProcess::splitCommand(command);
    QProcess::startDetached(args.takeFirst(), args);
#else
    QProcess::startDetached(command);
#endif
}
