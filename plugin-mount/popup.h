/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011-2013 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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

#ifndef POPUP_H
#define POPUP_H

#include <QDialog>
#include "../panel/ilxqtpanelplugin.h"
#include <Solid/Device>

class MenuDiskItem;
class QLabel;

class Popup: public QDialog
{
    Q_OBJECT
public:
    explicit Popup(ILxQtPanelPlugin *plugin, QWidget* parent = 0);

public slots:
    void showHide();

signals:
    void visibilityChanged(bool visible);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void deviceRemoved(QString const & udi);
    void deviceAdded(QString const & udi);

private:
    MenuDiskItem *addItem(Solid::Device device);

private:
    void realign();
    ILxQtPanelPlugin *mPlugin;
    QLabel *mPlaceholder;
    int mDisplayCount;
};

#endif // POPUP_H
