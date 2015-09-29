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

#include "pluginsettings.h"

PluginSettings::PluginSettings(LXQt::Settings* settings, const QString &group, QObject *parent)
    : QObject(parent),
    mSettings(settings),
    mOldSettings(settings),
    mGroup(group)
{
    connect(settings, &LXQt::Settings::settingsChanged, this, &PluginSettings::settingsChanged);
}

PluginSettings::~PluginSettings()
{
}

QVariant PluginSettings::value(const QString &key, const QVariant &defaultValue) const
{
    mSettings->beginGroup(mGroup);
    QVariant value = mSettings->value(key, defaultValue);
    mSettings->endGroup();
    return value;
}

void PluginSettings::setValue(const QString &key, const QVariant &value)
{
    mSettings->beginGroup(mGroup);
    mSettings->setValue(key, value);
    mSettings->endGroup();
}

void PluginSettings::remove(const QString &key)
{
    mSettings->beginGroup(mGroup);
    mSettings->remove(key);
    mSettings->endGroup();
}

bool PluginSettings::contains(const QString &key) const
{
    mSettings->beginGroup(mGroup);
    bool ret = mSettings->contains(key);
    mSettings->endGroup();
    return ret;
}

QList<QMap<QString, QVariant> > PluginSettings::readArray(const QString& prefix)
{
    mSettings->beginGroup(mGroup);
    QList<QMap<QString, QVariant> > array;
    int size = mSettings->beginReadArray(prefix);
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        QMap<QString, QVariant> hash;
        for (const QString &key : mSettings->childKeys())
            hash[key] = mSettings->value(key);
        array << hash;
    }
    mSettings->endArray();
    mSettings->endGroup();
    return array;
}

void PluginSettings::setArray(const QString &prefix, const QList<QMap<QString, QVariant> > &hashList)
{
    mSettings->beginGroup(mGroup);
    mSettings->beginWriteArray(prefix);
    int size = hashList.size();
    for (int i = 0; i < size; ++i)
    {
        mSettings->setArrayIndex(i);
        QMapIterator<QString, QVariant> it(hashList.at(i));
        while (it.hasNext())
        {
            it.next();
            mSettings->setValue(it.key(), it.value());
        }
    }
    mSettings->endArray();
    mSettings->endGroup();
}

void PluginSettings::clear()
{
    mSettings->beginGroup(mGroup);
    mSettings->clear();
    mSettings->endGroup();
}

void PluginSettings::sync()
{
    mSettings->beginGroup(mGroup);
    mSettings->sync();
    mOldSettings.loadFromSettings();
    mSettings->endGroup();
}

QStringList PluginSettings::allKeys() const
{
    mSettings->beginGroup(mGroup);
    QStringList keys = mSettings->allKeys();
    mSettings->endGroup();
    return keys;
}

void PluginSettings::loadFromCache()
{
    mSettings->beginGroup(mGroup);
    mOldSettings.loadToSettings();
    mSettings->endGroup();
}
