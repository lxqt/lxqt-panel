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
    enum OutputFormat_t {
        OUTPUT_BEGIN
            , OUTPUT_TEXT = OUTPUT_BEGIN
            , OUTPUT_ICON
            , OUTPUT_STRUCTURED
            , OUTPUT_END
    };

    static const QStringList msOutputFormatStrings;

public:
    explicit LXQtCustomCommandConfiguration(PluginSettings *settings, QWidget *parent = nullptr);
    ~LXQtCustomCommandConfiguration();

private slots:
    void autoRotateChanged(bool autoRotate);
    void fontButtonClicked();
    void textColorChanged();
    void textColorResetButtonClicked();
    void commandPlainTextEditChanged();
    void runWithBashCheckBoxChanged(bool runWithBash);
    void outputFormatComboBoxChanged(int index);
    void parseOnExitCheckBoxChanged(bool parseOnExit);
    void repeatCheckBoxChanged(bool repeat);
    void repeatTimerSpinBoxChanged();
    void iconLineEditChanged();
    void iconBrowseButtonClicked();
    void textLineEditChanged();
    void tooltipLineEditChanged();
    void maxWidthSpinBoxChanged();
    void clickLineEditChanged();
    void wheelUpLineEditChanged();
    void wheelDownLineEditChanged();

protected slots:
    virtual void loadSettings();

private:
    Ui::LXQtCustomCommandConfiguration *ui;
    bool mLockSettingChanges;
};

#endif // LXQTCUSTOMCOMMANDCONFIGURATION_H
