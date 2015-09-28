/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
 * Authors:
 *   Paulo Lieuthier <paulolieuthier@gmail.com>
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
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef PLUGIN_SETTINGS_H
#define PLUGIN_SETTINGS_H

#include <QObject>
#include <QString>
#include <LXQt/Settings>

class PluginSettings : public QObject
{
    Q_OBJECT

public:
    explicit PluginSettings(LXQt::Settings *settings, const QString &group, QObject *parent = nullptr);
    ~PluginSettings();

    QString group() const
    { return mGroup; }

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);

    void remove(const QString &key);
    bool contains(const QString &key) const;

    int beginReadArray(const QString &prefix);
    void beginWriteArray(const QString &prefix, int size = -1);
    void endArray();
    void setArrayIndex(int i);

    void clear();
    void sync();
    QStringList allKeys() const;

    void loadFromCache();

signals:
    void settingsChanged();

private:
    LXQt::Settings *mSettings;
    LXQt::SettingsCache mOldSettings;
    QString mGroup;
};

#endif
