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

#include "lxqtcustomcommand.h"
#include "custombutton.h"
#include "lxqtcustomcommandconfiguration.h"

#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <XdgIcon>
#include <LXQt/Globals>
#include <QDebug>

LXQtCustomCommand::LXQtCustomCommand(const ILXQtPanelPluginStartupInfo &startupInfo):
        QObject(),
        ILXQtPanelPlugin(startupInfo),
        mProcess(new QProcess(this)),
        mTimer(new QTimer(this)),
        mDelayedRunTimer(new QTimer(this)),
        mFirstRun(true),
        mOutput(QString()),
        mAutoRotate(true)
{
    mButton = new CustomButton(this);
    mButton->setObjectName(QLatin1String("CustomButton"));

    mFont = mButton->font().toString();

    mTimer->setSingleShot(true);
    mDelayedRunTimer->setSingleShot(true);
    mDelayedRunTimer->setInterval(500);

    connect(mButton, &CustomButton::clicked, this, &LXQtCustomCommand::handleClick);
    connect(mButton, &CustomButton::wheelScrolled, this, &LXQtCustomCommand::handleWheelScrolled);
    connect(mTimer, &QTimer::timeout, this, &LXQtCustomCommand::runCommand);
    connect(mDelayedRunTimer, &QTimer::timeout, this, &LXQtCustomCommand::runCommand);
    connect(mProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &LXQtCustomCommand::handleFinished);

    settingsChanged();
}

LXQtCustomCommand::~LXQtCustomCommand()
{
    // Ensure process is closed before exiting and avoids warning from QProcess.
    mProcess->close();
    delete mButton;
}

QWidget *LXQtCustomCommand::widget()
{
    return mButton;
}

void LXQtCustomCommand::realign()
{
    mButton->setAutoRotation(mAutoRotate);
}

QDialog *LXQtCustomCommand::configureDialog()
{
    if (!mConfigDialog)
        mConfigDialog = new LXQtCustomCommandConfiguration(settings());

    return mConfigDialog;
}

void LXQtCustomCommand::settingsChanged()
{
    bool shouldRun = false;

    bool oldAutoRotate = mAutoRotate;
    QString oldFont = mFont;
    QString oldCommand = mCommand;
    bool oldRunWithBash = mRunWithBash;
    bool oldOutputImage = mOutputImage;
    bool oldRepeat = mRepeat;
    int oldRepeatTimer = mRepeatTimer;
    QString oldIcon = mIcon;
    QString oldText = mText;
    int oldMaxWidth = mMaxWidth;

    mAutoRotate = settings()->value(QStringLiteral("autoRotate"), true).toBool();
    mFont = settings()->value(QStringLiteral("font"), QString()).toString(); // the default font should be empty
    mCommand = settings()->value(QStringLiteral("command"), QStringLiteral("echo Configure...")).toString();
    mRunWithBash = settings()->value(QStringLiteral("runWithBash"), true).toBool();
    mOutputImage = settings()->value(QStringLiteral("outputImage"), true).toBool();
    mRepeat = settings()->value(QStringLiteral("repeat"), true).toBool();
    mRepeatTimer = settings()->value(QStringLiteral("repeatTimer"), 5).toInt();
    mRepeatTimer = qMax(1, mRepeatTimer);
    mIcon = settings()->value(QStringLiteral("icon"), QString()).toString();
    mText = settings()->value(QStringLiteral("text"), QStringLiteral("%1")).toString();
    mMaxWidth = settings()->value(QStringLiteral("maxWidth"), 200).toInt();
    mClick = settings()->value(QStringLiteral("click"), QString()).toString();
    mWheelUp = settings()->value(QStringLiteral("wheelUp"), QString()).toString();
    mWheelDown = settings()->value(QStringLiteral("wheelDown"), QString()).toString();

    if (oldFont != mFont) {
        QFont newFont;
        if (!mFont.isEmpty()) // is empty when it's reset to app's font
            newFont.fromString(mFont);
        if (mFirstRun) {
            QTimer::singleShot(0, mButton, [this, newFont] {
                mButton->setFont(newFont);
                updateButton();
            });
        }
        else {
            mButton->setFont(newFont);
            updateButton();
        }
    }
    if (oldCommand != mCommand || oldRunWithBash != mRunWithBash || oldOutputImage != mOutputImage || oldRepeat != mRepeat)
        shouldRun = true;

    if (oldRepeatTimer != mRepeatTimer)
        mTimer->setInterval(mRepeatTimer * 1000);

    if (oldIcon != mIcon) {
        mButton->setIcon(XdgIcon::fromTheme(mIcon, QIcon(mIcon)));
        updateButton();
    }
    else if (oldText != mText)
        updateButton();

    if (oldMaxWidth != mMaxWidth)
        mButton->setMaxWidth(mMaxWidth);

    if (oldAutoRotate != mAutoRotate)
        mButton->setAutoRotation(mAutoRotate);

    if (mFirstRun) {
        mFirstRun = false;
        runCommand();
    }
    // Delay timer for running command, avoids multiple calls on settings change while typing command or clicking "Reset"
    else if (shouldRun)
        mDelayedRunTimer->start();
}

void LXQtCustomCommand::handleClick()
{
    if (!mClick.isEmpty())
        runDetached(mClick);
}

void LXQtCustomCommand::handleFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    if (exitCode == 0) {
        if(mOutputImage) {
            mOutputByteArray = mProcess->readAllStandardOutput();
        } else {
            mOutput = QString::fromUtf8(mProcess->readAllStandardOutput());
            if (mOutput.endsWith(QStringLiteral("\n")))
                mOutput.chop(1);
        }
    }
    else
        mOutput = tr("Error");

    updateButton();
    if (mRepeat)
        mTimer->start();
}

void LXQtCustomCommand::updateButton() {

    if(mOutputImage) {
        QPixmap pixmap;
        pixmap.loadFromData(mOutputByteArray);
        QIcon icon(pixmap);
        mButton->setIcon(icon);
        mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    } else {
        QString newText = mText;
        if (newText.contains(QStringLiteral("%1")))
            newText = newText.arg(mOutput);

        mButton->setText(newText);

        if (mButton->icon().isNull())
             mButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
        else
             mButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }

    mButton->updateWidth();
}

void LXQtCustomCommand::handleWheelScrolled(int yDelta)
{
    if (yDelta > 0 && !mWheelUp.isEmpty())
        runDetached(mWheelUp);
    else if (yDelta < 0 && !mWheelDown.isEmpty())
        runDetached(mWheelDown);
}

void LXQtCustomCommand::runCommand()
{
    if (mCommand.isEmpty())
        return;

    if (mProcess->state() != QProcess::NotRunning) {
        mDelayedRunTimer->start();
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

void LXQtCustomCommand::runDetached(QString command)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5,15,0))
    QStringList args = QProcess::splitCommand(command);
    QProcess::startDetached(args.takeFirst(), args);
#else
    QProcess::startDetached(command);
#endif
}
