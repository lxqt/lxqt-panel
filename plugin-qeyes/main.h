/*
 *  qeyes - an xeyes clone
 *
 *  Copyright (C) 2022 Goffredo Baroncelli <kreijack@inwind.it>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
 
#include <iostream>

#include <QtWidgets/QApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFile>
#include <QtCore/QTextStream>


#include <lxqt/ilxqtpanelplugin.h>

#include "qeyesvectorwidget.h"
#include "qeyesimagewidget.h"

class QEyesPlugin :  public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT

public:
    QEyesPlugin(const ILXQtPanelPluginStartupInfo &startupInfo);

    virtual QWidget *widget() override { return w; }
    virtual QString themeId()  const override{
        return QStringLiteral("QEyesPlugin");
    }
    virtual void realign() override;

private:
    QEyesVectorWidget *w;
};

class QEyesPluginLibrary: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxqt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const;
};


