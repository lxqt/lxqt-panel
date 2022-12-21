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
#include "qeyes.h"

QEyesConfigDialog::QEyesConfigDialog(PluginSettings *sts,
        ILXQtPanelPlugin *plugin_, QWidget *parent) : 
    QDialog(parent), _settings(sts), plugin(plugin_)
{

    auto l1 = new QGridLayout();
    setLayout(l1);

    auto f = new QGroupBox(tr("Appearance"));
    l1->addWidget(f, 10, 10, 10, 11);

    auto l = new QGridLayout();
    f->setLayout(l);

    l->addWidget(new QLabel(tr("Number of eyes")), 10, 10);
    numEyesWidget = new QSpinBox();
    l->addWidget(numEyesWidget, 10, 20);
    numEyesWidget->setMaximum(10);
    numEyesWidget->setMinimum(1);

    l->addWidget(new QLabel(tr("Type")), 20, 10);
    typesWidget = new QComboBox();
    l->addWidget(typesWidget, 20, 20);

    auto b = new QPushButton(tr("Close"));
    l1->addWidget(b, 50, 10);
    connect(b, &QPushButton::clicked, this, &QEyesConfigDialog::updateAndClose);
    b->setDefault(true);
    
    b = new QPushButton(tr("Reset"));
    l1->addWidget(b, 50, 20);
    connect(b, &QPushButton::clicked, this, &QEyesConfigDialog::resetValue);

    setWindowTitle(tr("QEyes setting"));

    connect(numEyesWidget, qOverload<int>(&QSpinBox::valueChanged),
        this, &QEyesConfigDialog::updateValues);
    connect(typesWidget, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &QEyesConfigDialog::updateValues);

}

void QEyesConfigDialog::showEvent(QShowEvent *) {
    old_num_eyes = _settings->value(QStringLiteral("num_eyes"),
                                QStringLiteral("2")).toInt();
    old_type_eyes = _settings->value(QStringLiteral("eye_type"),
                                QEyesPlugin::internalEye).toString();
    buildList();
    bool found = false;
    for (const auto &key : types.keys()) {
        if (old_type_eyes == types[key]) {
            found = true;
            break;
        }
    }
    if (!found)
        old_type_eyes = QEyesPlugin::internalEye;

    typesWidget->blockSignals(true);
    numEyesWidget->blockSignals(true);

    typesWidget->clear();
    typesWidget->addItem(tr("QEyes default"));
    for (const auto &key: types.keys())
        typesWidget->addItem(key);

    resetValue();

    adjustSize();

    typesWidget->blockSignals(false);
    numEyesWidget->blockSignals(false);
}

void QEyesConfigDialog::resetValue() {
    int actIndex = 0;
    int c = 1;  // 0 is <internal>
    for (const auto &key : types.keys()) {
        if (old_type_eyes == types[key])
            actIndex = c;
        c++;
    }

    typesWidget->setCurrentIndex(actIndex);
    numEyesWidget->setValue(old_num_eyes);
}

void QEyesConfigDialog::updateValues(int) {
    _settings->setValue(QStringLiteral("num_eyes"),
        numEyesWidget->value());

    if (typesWidget->currentIndex() == 0 ||
        !types.contains(typesWidget->currentText())) {
            _settings->setValue(QStringLiteral("eye_type"), QEyesPlugin::internalEye);
    } else {
            _settings->setValue(QStringLiteral("eye_type"),
                types[typesWidget->currentText()]);
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
    for (const char *root : { LXQT_SHARE_DIR "/panel/qeyes-types/",
                              "$HOMEDATA"
                              }) {

        if (!strcmp(root, "$HOMEDATA")) {
            static char buf[1024];

            if (!getenv("XDG_DATA_HOME"))
                continue;
            snprintf(buf, 1023, "%s/lxqt/panel/qeyes-types/", getenv("XDG_DATA_HOME"));
            root = buf;
        }

        auto dir = QDir(QLatin1String(root));
        auto list = dir.entryList();
        for (auto const & path : list) {
            const auto fn = QString(QLatin1String(root) + QStringLiteral("/") +
                                path + QStringLiteral("/config"));
            QFileInfo f(fn);

            if (fn == QStringLiteral(".") || fn == QStringLiteral(".."))
                continue;
            if (!f.exists())
                continue;
            if (!f.isFile())
                continue;

            types[path] = 
                QString(QLatin1String(root) + QStringLiteral("/") + path);
        }
    }
}
