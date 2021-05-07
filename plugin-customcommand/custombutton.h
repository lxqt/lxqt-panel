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

#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QToolButton>

#include "../panel/ilxqtpanel.h"
#include "../panel/ilxqtpanelplugin.h"

class CustomButton : public QToolButton
{
    Q_OBJECT

public:
    CustomButton(ILXQtPanelPlugin *plugin, QWidget* parent = nullptr);
    ~CustomButton();

public slots:
    void setAutoRotation(bool value);
    void setMaxWidth(int maxWidth);
    void updateWidth();

protected:
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void setOrigin(Qt::Corner newOrigin);

private:
    ILXQtPanelPlugin *mPlugin;
    ILXQtPanel *mPanel;
    Qt::Corner mOrigin;
    int mMaxWidth;

signals:
    void wheelScrolled(int);
};

#endif // CUSTOMBUTTON_H
