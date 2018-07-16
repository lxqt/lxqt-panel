/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
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


#ifndef LXQTSYSSTATCOLOURS_HPP
#define LXQTSYSSTATCOLOURS_HPP

#include <QDialog>

#include <QMap>
#include <QString>
#include <QColor>


namespace Ui {
    class LXQtSysStatColors;
}

class QSignalMapper;
class QAbstractButton;
class QPushButton;

class LXQtSysStatColors : public QDialog
{
    Q_OBJECT

public:
    explicit LXQtSysStatColors(QWidget *parent = NULL);
    ~LXQtSysStatColors();

    typedef QMap<QString, QColor> Colors;

    void setColors(const Colors&);

    Colors colours() const;

    Colors defaultColors() const;

signals:
    void coloursChanged();

public slots:
    void on_buttons_clicked(QAbstractButton*);

    void selectColor(const QString &);

    void restoreDefaults();
    void reset();
    void apply();

private:
    Ui::LXQtSysStatColors *ui;

    QSignalMapper *mSelectColorMapper;
    QMap<QString, QPushButton*> mShowColorMap;

    Colors mDefaultColors;
    Colors mInitialColors;
    Colors mColors;

    void applyColorsToButtons();
};

#endif // LXQTSYSSTATCOLOURS_HPP
