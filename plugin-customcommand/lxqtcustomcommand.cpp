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

#include <QByteArray>
#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfo>
#include <LXQt/Globals>

#include <algorithm>

LXQtCustomCommand::LXQtCustomCommand(const ILXQtPanelPluginStartupInfo &startupInfo):
        QObject(),
        ILXQtPanelPlugin(startupInfo),
        mProcess(new QProcess(this)),
        mTerminating(false),
        mTimer(new QTimer(this)),
        mDelayedRunTimer(new QTimer(this)),
        mFirstRun(true),
        mAutoRotate(true),
        mRunWithBash(true),
        mOutputFormat(OutputFormat_t::OUTPUT_BEGIN),
        mParseOnExit(true),
        mRepeat(true),
        mRepeatTimer(5),
        mMaxWidth(200)
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
    connect(mProcess, &QProcess::finished, this, &LXQtCustomCommand::handleFinished);
    connect(mProcess, &QProcess::readyReadStandardOutput, this, &LXQtCustomCommand::handleOutput);

    settingsChanged();
}

LXQtCustomCommand::~LXQtCustomCommand()
{
    // Ensure process is closed before exiting and avoids warning from QProcess.
    mTerminating = true;
    mProcess->terminate();
    delete mButton;
    mProcess->waitForFinished(200);
    mProcess->close();
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
    LXQtCustomCommandConfiguration::OutputFormat_t oldOutputFormat = mOutputFormat;
    bool oldParseOnExit = mParseOnExit;
    bool oldRepeat = mRepeat;
    int oldRepeatTimer = mRepeatTimer;
    QString oldIcon = mIcon;
    QString oldText = mText;
    QString oldTooltip = mTooltip;
    int oldMaxWidth = mMaxWidth;

    mAutoRotate = settings()->value(QStringLiteral("autoRotate"), true).toBool();
    mFont = settings()->value(QStringLiteral("font"), QString()).toString(); // the default font should be empty
    QColor textColor = QColor::fromString(settings()->value(QStringLiteral("textColor")).toString());
    mCommand = settings()->value(QStringLiteral("command"), QStringLiteral("echo Configure...")).toString().trimmed();
    mRunWithBash = settings()->value(QStringLiteral("runWithBash"), true).toBool();
    // backward compatibility check
    if (settings()->contains(QStringLiteral("outputFormat")))
        mOutputFormat = static_cast<LXQtCustomCommandConfiguration::OutputFormat_t>(settings()->value(QStringLiteral("outputFormat")).toInt());
    else
        mOutputFormat = settings()->value(QStringLiteral("outputImage"), false).toBool() ? OutputFormat_t::OUTPUT_ICON : OutputFormat_t::OUTPUT_TEXT;
    mParseOnExit = settings()->value(QStringLiteral("parseOnExit"), true).toBool();
    mRepeat = settings()->value(QStringLiteral("repeat"), true).toBool();
    mRepeatTimer = settings()->value(QStringLiteral("repeatTimer"), 5).toInt();
    mRepeatTimer = std::max(1, mRepeatTimer);
    mIcon = settings()->value(QStringLiteral("icon"), QString()).toString();
    mText = settings()->value(QStringLiteral("text"), QStringLiteral("%1")).toString();
    mTooltip = settings()->value(QStringLiteral("tooltip"), QString()).toString();
    mMaxWidth = settings()->value(QStringLiteral("maxWidth"), 200).toInt();
    mClick = settings()->value(QStringLiteral("click"), QString()).toString().trimmed();
    mWheelUp = settings()->value(QStringLiteral("wheelUp"), QString()).toString().trimmed();
    mWheelDown = settings()->value(QStringLiteral("wheelDown"), QString()).toString().trimmed();

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
    if (textColor.isValid()) {
        mButton->setStyleSheet(QStringLiteral("QToolButton{color: %1}").arg(textColor.name()));
    }
    else {
        mButton->setStyleSheet(QString());
    }
    if (oldCommand != mCommand || oldRunWithBash != mRunWithBash || oldOutputFormat != mOutputFormat || oldParseOnExit != mParseOnExit || oldRepeat != mRepeat)
        shouldRun = true;

    if (mFirstRun || oldRepeatTimer != mRepeatTimer)
        mTimer->setInterval(mRepeatTimer * 1000);

    if (oldIcon != mIcon || (oldOutputFormat != OutputFormat_t::OUTPUT_TEXT && mOutputFormat == OutputFormat_t::OUTPUT_TEXT)) {
        mButton->setIcon(QIcon::fromTheme(mIcon, QIcon(mIcon)));
        updateButton();
    }
    else if (oldText != mText || oldTooltip != mTooltip)
        updateButton();

    if (mFirstRun || oldMaxWidth != mMaxWidth)
        mButton->setMaxWidth(mMaxWidth);

    if (mFirstRun || oldAutoRotate != mAutoRotate)
        mButton->setAutoRotation(mAutoRotate);

    if (mFirstRun) {
        mFirstRun = false;
        shouldRun = true;
    }
    // Delay timer for running command, avoids multiple calls on settings change while typing command or clicking "Reset"
    if (shouldRun) {
        if (mProcess->state() == QProcess::Running) {
            mTerminating = true;
            mProcess->terminate();
        }
        mDelayedRunTimer->start();
    }
}

void LXQtCustomCommand::handleClick()
{
    if (!mClick.isEmpty())
        runDetached(mClick);
}

void LXQtCustomCommand::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (!mTerminating)
    {
        if (exitStatus != QProcess::NormalExit || exitCode != 0)
            qWarning().nospace() << "customcommand: non-gracefull command finish(" << exitStatus << ',' << exitCode << "): " << mProcess->readAllStandardError();
        else if (mParseOnExit) {
            mOutputByteArray = mProcess->readAllStandardOutput();
            updateButton();
        }
        if (mRepeat)
            mTimer->start();
    }
}

void LXQtCustomCommand::handleOutput()
{
    if (mParseOnExit) 
        return;

    bool something_read = false;
    while (mProcess->canReadLine()) {
        mOutputByteArray = mProcess->readLine();
        something_read = true;
    }

    if (something_read)
        updateButton();
}


void LXQtCustomCommand::updateButton() {
    const auto iconsetter = [this](const QByteArray & iconData, const bool decoded) {
        QIcon icon;
        if (iconData.size() > 0) {
            const QString iconString = QString::fromUtf8(iconData.trimmed());
            static const QRegularExpression re_no_xdg_name{QStringLiteral("[^[:alnum:]-]")};
            if (!iconString.contains(re_no_xdg_name))
                icon = QIcon::fromTheme(iconString);
            if (icon.isNull() && !iconString.contains(QChar::Null) && QFileInfo::exists(iconString))
                icon = QIcon{iconString};
            if (icon.isNull()) {
                QPixmap pixmap;
                pixmap.loadFromData(iconData);
                if (pixmap.isNull() && !decoded)
                    pixmap.loadFromData(QByteArray::fromBase64(iconData));
                icon = QIcon{pixmap};
            }
        }
        mButton->setIcon(icon);
    };
    switch (mOutputFormat) {
        case OutputFormat_t::OUTPUT_STRUCTURED:
            if (mOutputByteArray.size() > 0) {
                for (const auto & variable : mOutputByteArray.split(' ')) {
                    const auto & name_value = variable.split(':');
                    bool error = false;
                    QByteArray value;
                    if (name_value.size() != 2)
                        error = true;
                    if (!error) {
                        const auto decoded = QByteArray::fromBase64Encoding(name_value[1]);
                        if (decoded.decodingStatus == QByteArray::Base64DecodingStatus::Ok)
                            value = decoded.decoded;
                        else
                            error = true;
                    }
                    if (error) {
                        qWarning().nospace() << "customcommand: Can't parse name-value(" << (name_value.empty() ? QString{} : QString::fromUtf8(name_value[0]))
                            << ") from input: " << QString::fromUtf8(mOutputByteArray);
                        continue;
                    }
                    if (name_value[0] == "text") {
                        mButton->setText(QString::fromUtf8(value));
                    } else if (name_value[0] == "tooltip") {
                        mButton->setToolTip(QString::fromUtf8(value));
                    } else if (name_value[0] == "icon") {
                        iconsetter(value, true);
                    } else {
                        qWarning().nospace() << "customcommand: Unsupported parameter(" << QString::fromUtf8(name_value[0]) << ") to set";
                    }
                }
            }
            break;
        case OutputFormat_t::OUTPUT_ICON:
            iconsetter(mOutputByteArray, false);
            mButton->setText(QString{});
            break;
        case OutputFormat_t::OUTPUT_TEXT:
            {
                QString newText = mText;
                if (newText.contains(QStringLiteral("%1")))
                    newText = newText.arg(QString::fromUtf8(mOutputByteArray.trimmed()));

                mButton->setText(newText);
            }
            break;
        case OutputFormat_t::OUTPUT_END:
            assert(false);
    }
    if (mButton->icon().isNull())
         mButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    else if (mButton->text().isEmpty())
        mButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    else
         mButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    mButton->updateWidth();
}

void LXQtCustomCommand::handleWheelScrolled(int delta)
{
    if (delta > 0 && !mWheelUp.isEmpty())
        runDetached(mWheelUp);
    else if (delta < 0 && !mWheelDown.isEmpty())
        runDetached(mWheelDown);
}

void LXQtCustomCommand::runCommand()
{
    if (mProcess->state() == QProcess::Running)
        mProcess->close();

    mTerminating = false;

    if (mCommand.isEmpty())
        return;

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
