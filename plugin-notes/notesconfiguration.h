/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Matteo Fois <giomatfois62@yahoo.it>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef NOTESCONFIGURATION_H
#define NOTESCONFIGURATION_H

#include <QDialog>

#include "../panel/lxqtpanelpluginconfigdialog.h"
#include "../panel/pluginsettings.h"

namespace Ui {
class NotesConfiguration;
}

class NotesConfiguration : public LXQtPanelPluginConfigDialog
{
    Q_OBJECT

public:
    explicit NotesConfiguration(PluginSettings *settings, QWidget *parent = nullptr);
    ~NotesConfiguration();
    
public slots:
    void on_changeFont_clicked(bool checked = true);
    void on_bgColorButton_clicked(bool checked = true);
    void on_fgColorButton_clicked(bool checked = true);
    void on_showWindowFrame_toggled(bool checked);
    void on_showNotesOnStartup_toggled(bool checked);

private slots:
    void loadSettings();

private:
    Ui::NotesConfiguration *ui;
    
    void setBgColor(const QColor &color);
    void setFgColor(const QColor &color);
    void setFont(const QFont &font);
};

#endif // NOTESCONFIGURATION_H
