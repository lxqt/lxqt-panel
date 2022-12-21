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
#include <map>
#include <string>


#include <QtWidgets/QDialog>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QComboBox>
#include <QtCore/QMap>


#include "../panel/pluginsettings.h"
#include "../panel/ilxqtpanelplugin.h"

class QEyesConfigDialog : public QDialog {
private:
    Q_OBJECT
public:
    QEyesConfigDialog(PluginSettings *_settings,
        ILXQtPanelPlugin *plugin, QWidget *parent = nullptr);
private:
    QSpinBox *numEyesWidget;
    QComboBox *typesWidget;
    PluginSettings *_settings;
    QMap<QString, QString>   types;
    ILXQtPanelPlugin *plugin;

    void buildList();
    void resetValue();
    void updateAndClose();
    
    int old_num_eyes;
    QString old_type_eyes;

    void showEvent(QShowEvent *) override;

public slots:
    void updateValues(int);

};
