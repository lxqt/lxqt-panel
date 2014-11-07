/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2014 LXQt team
 * Authors:
 *   Jes <zjesclean.gmail@gmail.com>
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

#include <QDebug>
#include <QSettings>
#include "lxqtkbdlayoutconf.h"
#include "ui_lxqtkbdlayoutconf.h"
#include "lxqtkbdlayout.h"

LxQtKbdLayoutConf::LxQtKbdLayoutConf(QSettings *set) :
    QDialog(0),
    m_ui(new Ui::LxQtKbdLayoutConf),
    m_settings(set)
{
    setAttribute(Qt::WA_DeleteOnClose);

    m_ui->setupUi(this);

    SwitchMode mode =
        static_cast<SwitchMode>(m_settings->value("switchmode").toInt());

    switch(mode){
    case SwitchMode::Global:
        m_ui->globMode->setChecked(true);
        break;
    case SwitchMode::Application:
        m_ui->appMode->setChecked(true);
        break;
    case SwitchMode::Window:
        m_ui->wndMode->setChecked(true);
        break;
    }
    m_ui->showCaps->setChecked(m_settings->value("showcaps", true).toBool());

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &LxQtKbdLayoutConf::onAccept);
}

void LxQtKbdLayoutConf::onAccept()
{
    SwitchMode mode = SwitchMode::Window;
    if (m_ui->globMode->isChecked())
        mode = SwitchMode::Global;
    else if (m_ui->appMode->isChecked())
        mode = SwitchMode::Application;

    m_settings->setValue("switchmode", static_cast<int>(mode));
    m_settings->setValue("showcaps", m_ui->showCaps->isChecked());
}

LxQtKbdLayoutConf::~LxQtKbdLayoutConf()
{
    delete m_ui;
}
