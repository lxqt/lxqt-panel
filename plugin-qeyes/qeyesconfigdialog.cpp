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

#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "qeyesconfigdialog.h"

QEyesConfigDialog::QEyesConfigDialog(PluginSettings *sts,
        ILXQtPanelPlugin *plugin_, QWidget *parent) : 
    QDialog(parent), _settings(sts), plugin(plugin_)
{

    old_num_eyes = _settings->value(QLatin1String("num_eyes"),
                                QLatin1String("2")).toInt();
    old_type_eyes = _settings->value(QLatin1String("eye_type"),
                                QLatin1String("<internal>")).toString();
    buildList();
    bool found = false;
    for (auto it = types.begin() ; it != types.end() ; ++it) {
        if (old_type_eyes == it.value()) {
            found = true;
            break;
        }
    }
    if (!found)
        old_type_eyes = QLatin1String("<internal>");
    
    auto l1 = new QGridLayout();
    setLayout(l1);

    auto f = new QGroupBox(QString::fromUtf8("Appearance"));
    l1->addWidget(f, 10, 10, 10, 11);

    auto l = new QGridLayout();
    f->setLayout(l);

    l->addWidget(new QLabel(QString::fromUtf8("Number of eyes")), 10, 10);
    numEyesWidget = new QSpinBox();
    l->addWidget(numEyesWidget, 10, 20);
    numEyesWidget->setMaximum(10);
    numEyesWidget->setMinimum(1);

    l->addWidget(new QLabel(QString::fromUtf8("Type")), 20, 10);
    typesWidget = new QComboBox();
    l->addWidget(typesWidget, 20, 20);

    typesWidget->addItem(QLatin1String("<Internal>"));
    for (auto it = types.begin() ; it != types.end() ; ++it)
        typesWidget->addItem(it.key());

    auto b = new QPushButton(QString::fromUtf8("Close"));
    l1->addWidget(b, 50, 10);
    connect(b, &QPushButton::clicked, this, &QEyesConfigDialog::updateAndClose);
    b->setDefault(true);
    
    b = new QPushButton(QString::fromUtf8("Reset"));
    l1->addWidget(b, 50, 20);
    connect(b, &QPushButton::clicked, this, &QEyesConfigDialog::resetValue);

    setWindowTitle(QString::fromUtf8("QEyes setting"));
    resetValue();

    connect(numEyesWidget, qOverload<int>(&QSpinBox::valueChanged),
        this, &QEyesConfigDialog::updateValues);
    connect(typesWidget, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &QEyesConfigDialog::updateValues);

}

void QEyesConfigDialog::resetValue() {
    int actIndex = 0;
    int c = 0;
    for (auto it = types.begin() ; it != types.end() ; ++it, c++) {
        if (it.value() == old_type_eyes)
            actIndex = c;
    }
    typesWidget->setCurrentIndex(actIndex);

    numEyesWidget->setValue(old_num_eyes);
}

void QEyesConfigDialog::updateValues(int) {
    _settings->setValue(QLatin1String("num_eyes"),
        numEyesWidget->value());
    
    if (typesWidget->currentIndex() == 0) {
        _settings->setValue(QLatin1String("eye_type"), QLatin1String("<internal>"));
    } else {
        _settings->setValue(QLatin1String("eye_type"), types[typesWidget->currentText()]);
    }
    _settings->sync();
    plugin->settingsChanged();
}

void QEyesConfigDialog::updateAndClose() {
    updateValues(0);
    done(QDialog::Accepted);
}

void QEyesConfigDialog::buildList() {
    types.clear();
    for (const char *root : { "/usr/share/gnome-applets/geyes",
                              "/usr/local/share/gnome-applets/geyes",
                              "/usr/share/lxqt/applets/qeyes",
                              "/usr/local/share/lxqt/applets/qeyes",
                              "/usr/share/mate-applets/geyes",
                              "/usr/local/share/mate-applets/geyes" }) {

        auto dir = QDir(QLatin1String(root));
        auto list = dir.entryList();
        for (auto const & path : list) {
            const auto fn = QString(QLatin1String(root) + QLatin1String("/") +
                                path + QLatin1String("/config"));
            QFileInfo f(fn);
            
            if (!f.exists())
                continue;
            if (!f.isFile())
                continue;

            types.insert(path,
                QString(QLatin1String(root) + QLatin1String("/") + path));
        }
    }
}
