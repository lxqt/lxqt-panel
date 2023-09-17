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

#ifndef LXQTCUSTOMCOMMANDCONFIGURATION_H
#define LXQTCUSTOMCOMMANDCONFIGURATION_H

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

namespace Ui {
    class LXQtCustomCommandConfiguration;
}

class LXQtCustomCommandConfiguration : public LXQtPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit LXQtCustomCommandConfiguration(PluginSettings *settings, QWidget *parent = nullptr);
    ~LXQtCustomCommandConfiguration();

private slots:
    void autoRotateChanged(bool autoRotate);
    void fontButtonClicked();
    void commandPlainTextEditChanged();
    void runWithBashCheckBoxChanged(bool runWithBash);
    void outputImageCheckBoxChanged(bool outputImage);
    void repeatCheckBoxChanged(bool repeat);
    void repeatTimerSpinBoxChanged(int repeatTimer);
    void iconLineEditChanged(QString icon);
    void iconBrowseButtonClicked();
    void textLineEditChanged(QString text);
    void maxWidthSpinBoxChanged(int maxWidth);
    void clickLineEditChanged(QString click);
    void wheelUpLineEditChanged(QString wheelUp);
    void wheelDownLineEditChanged(QString wheelDown);

protected slots:
    virtual void loadSettings();

private:
    Ui::LXQtCustomCommandConfiguration *ui;
    bool mLockSettingChanges;
};

#endif // LXQTCUSTOMCOMMANDCONFIGURATION_H
