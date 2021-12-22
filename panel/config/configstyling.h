/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
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

#ifndef CONFIGSTYLING_H
#define CONFIGSTYLING_H

#include "../lxqtpanel.h"
#include <QSettings>
#include <QTimer>
#include <LXQt/ConfigDialog>

class LXQtPanel;

namespace Ui {
    class ConfigStyling;
}

class ConfigStyling : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigStyling(LXQtPanel *panel, QWidget *parent = nullptr);
    ~ConfigStyling();

    void updateIconThemeSettings();

signals:
    void changed();

public slots:
    void reset();

private slots:
    void editChanged();
    void pickFontColor();
    void pickBackgroundColor();
    void pickBackgroundImage();

private:
    Ui::ConfigStyling *ui;
    LXQtPanel *mPanel;

    void fillComboBox_icon();

    // new values
    QColor mFontColor;
    QColor mBackgroundColor;

    // old values for reset
    QColor mOldFontColor;
    QColor mOldBackgroundColor;
    QString mOldBackgroundImage;
    int mOldOpacity;
};

#endif
