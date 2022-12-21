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

#include <stdio.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialog>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "../panel/pluginsettings.h"

#include "qeyesconfigdialog.h"
#include "qeyes.h"

ILXQtPanelPlugin *QEyesPluginLibrary::instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
{
    return new QEyesPlugin(startupInfo);
}

QEyesPlugin::QEyesPlugin(const ILXQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILXQtPanelPlugin(startupInfo)
{
    w0 = new QWidget();
    l = new QVBoxLayout();
    l->setSpacing(0);
    l->setMargin(0);
    l->setContentsMargins (0, 0, 0, 0);
    w0->setLayout(l);

    w = new QEyesVectorWidget();
    l->addWidget(w);
    w->setTransparent(true);
    vectorEyes = true;

    settingsChanged();
    realign();
}

void QEyesPlugin::realign() {
    const auto g = panel()->globalGeometry();
    if (panel()->isHorizontal()) {
        w->setMinimumHeight(g.height() / 2);
        w->setMinimumWidth(0.8 * g.height() * w->getNumEyes() );
    } else {
        w->setMinimumWidth(g.width() / 2);
        w->setMinimumHeight(g.width());
    }
    w->update();
}


static bool loadImage(QString path, QEyesImageWidget *w) {
    QFile file(path + QStringLiteral("/config"));
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString eye, pupil;
    int num=1, wall=1;

    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList  fields = line.split(QLatin1String("="));
        const auto name = fields.at(0).trimmed();
        auto value = fields.at(1).trimmed();
        if (value.size() > 1 && value.at(0) == QChar(QLatin1Char('"')))
            value = value.mid(1, value.size() - 2);
        if (name == QStringLiteral("wall-thickness")) {
            wall = value.toInt();
        } else if (name == QStringLiteral("eye-pixmap")) {
            eye = path + QStringLiteral("/") + value;
        } else if (name == QStringLiteral("pupil-pixmap")) {
            pupil = path + QStringLiteral("/") + value;
        } else if (name.trimmed() == QStringLiteral("num-eyes")) {
            num = value.toInt();
        }
    }

    return w->load(eye, pupil, wall, num);
}

void QEyesPlugin::settingsChanged() {
    PluginSettings *_settings = settings();

    const auto type = _settings->value(QStringLiteral("eye_type"),
                            internalEye).toString();

    if (type == internalEye && !vectorEyes) {
        l->removeWidget(w);
        delete w;
        w = new QEyesVectorWidget();
        l->addWidget(w);
        w->setTransparent(true);
        vectorEyes = true;
    } else if (type != internalEye && vectorEyes) {
        l->removeWidget(w);
        delete w;
        w = new QEyesImageWidget();
        l->addWidget(w);
        w->setTransparent(true);
        vectorEyes = false;
    }

    if (type != internalEye && !vectorEyes) {
        if (!loadImage(type, dynamic_cast<QEyesImageWidget*>(w))) {
            l->removeWidget(w);
            delete w;
            w = new QEyesVectorWidget();
            l->addWidget(w);
            w->setTransparent(true);
            vectorEyes = true;

            _settings->setValue(QStringLiteral("eye_type"),
                                internalEye);
            std::cerr << "ERROR: crash during load image\n" ;
        }
    }

    w->setNumEyes(_settings->value(QStringLiteral("num_eyes"),
                            QLatin1String("2")).toInt());
    realign();
}

QDialog * QEyesPlugin::configureDialog() {
    return new QEyesConfigDialog(settings(), this);
}

const QString QEyesPlugin::internalEye = QStringLiteral("<internal>");
