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

#ifndef LXQTCUSTOMCOMMAND_H
#define LXQTCUSTOMCOMMAND_H

#include "../panel/ilxqtpanelplugin.h"

#include <QToolButton>
#include <QTimer>
#include <QProcess>
#include <QPointer>
#include <LXQt/RotatedWidget>

class CustomButton;
class LXQtCustomCommandConfiguration;

class LXQtCustomCommand : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    LXQtCustomCommand(const ILXQtPanelPluginStartupInfo &startupInfo);
    ~LXQtCustomCommand();

    virtual QWidget *widget();
    virtual QString themeId() const { return QStringLiteral("Custom"); }
    virtual ILXQtPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog ; }
    void realign();
    QDialog *configureDialog();

protected slots:
    virtual void settingsChanged();

private slots:
    void handleClick();
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleWheelScrolled(int yDelta);
    void updateButton();
    void runCommand();
    void runDetached(QString command);


private:
    CustomButton *mButton;
    QPointer<LXQtCustomCommandConfiguration> mConfigDialog;

    QProcess *mProcess;
    QTimer *mTimer;
    QTimer *mDelayedRunTimer;

    bool mFirstRun;
    QString mOutput;
    QByteArray mOutputByteArray;

    bool mAutoRotate;
    QString mFont;
    QString mCommand;
    bool mRunWithBash;
    bool mOutputImage;
    bool mRepeat;
    int mRepeatTimer;
    QString mIcon;
    QString mText;
    int mMaxWidth;
    QString mClick;
    QString mWheelUp;
    QString mWheelDown;
};


class LXQtCustomCommandPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    {
        return new LXQtCustomCommand(startupInfo);
    }
};

#endif // LXQTCUSTOMCOMMAND_H
