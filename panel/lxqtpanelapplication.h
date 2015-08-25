/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXDE-Qt - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
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


#ifndef LXQTPANELAPPLICATION_H
#define LXQTPANELAPPLICATION_H

#include <LXQt/Application>
#include "ilxqtpanelplugin.h"

class QScreen;

class LXQtPanel;
namespace LXQt {
class Settings;
}

class LXQtPanelApplication : public LXQt::Application
{
    Q_OBJECT
public:
    explicit LXQtPanelApplication(int& argc, char** argv);
    ~LXQtPanelApplication();

    int count() { return mPanels.count(); }
    LXQt::Settings *settings() { return mSettings; }
    bool isPluginSingletonAndRunnig(QString const & pluginId) const;

public slots:
    void addNewPanel();

signals:
    void pluginAdded();
    void pluginRemoved();

private:
    QList<LXQtPanel*> mPanels;

    LXQtPanel* addPanel(const QString &name);

private slots:
    void removePanel(LXQtPanel* panel);

    void handleScreenAdded(QScreen* newScreen);
    void screenDestroyed(QObject* screenObj);
    void reloadPanelsAsNeeded();
    void cleanup();

private:
    LXQt::Settings *mSettings;
};


#endif // LXQTPANELAPPLICATION_H
