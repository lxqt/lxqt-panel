/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * http://lxqt.org
 *
 * Copyright: 2015 LXQt team
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

#include "panelpluginsmodel.h"
#include "plugin.h"
#include "ilxqtpanelplugin.h"
#include "lxqtpanel.h"
#include <QPointer>
#include <XdgIcon>
#include <LXQt/Settings>

#include <QDebug>

PanelPluginsModel::PanelPluginsModel(LxQtPanel * panel,
                                     QString const & namesKey,
                                     QStringList const & desktopDirs,
                                     QObject * parent/* = nullptr*/)
    : QAbstractListModel{parent},
    mNamesKey(namesKey),
    mPanel(panel)
{
    loadPlugins(desktopDirs);
}

PanelPluginsModel::~PanelPluginsModel()
{
    qDeleteAll(plugins());
}

int PanelPluginsModel::rowCount(const QModelIndex & parent/* = QModelIndex()*/) const
{
    return QModelIndex() == parent ? mPlugins.size() : 0;
}


QVariant PanelPluginsModel::data(const QModelIndex & index, int role/* = Qt::DisplayRole*/) const
{
    Q_ASSERT(QModelIndex() == index.parent()
            && 0 == index.column()
            && mPlugins.size() > index.row()
            );

    pluginslist_t::const_reference plugin = mPlugins[index.row()];
    QVariant ret;
    switch (role)
    {
        case Qt::DisplayRole:
            if (plugin.second.isNull())
                ret = QStringLiteral("<b>Unknown</b> (%1)").arg(plugin.first);
            else
                ret = QStringLiteral("<b>%1</b> (%2)").arg(plugin.second->name(), plugin.first);
            break;
        case Qt::DecorationRole:
            if (plugin.second.isNull())
                ret = XdgIcon::fromTheme("preferences-plugin");
            else
                ret = plugin.second->desktopFile().icon(XdgIcon::fromTheme("preferences-plugin"));
            break;
        case Qt::UserRole:
            ret = QVariant::fromValue(const_cast<Plugin const *>(plugin.second.data()));
            break;
    }
    return ret;
}

Qt::ItemFlags PanelPluginsModel::flags(const QModelIndex & index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

QStringList PanelPluginsModel::pluginNames() const
{
    QStringList names;
    for (auto const & p : mPlugins)
        names.append(p.first);
    return std::move(names);
}

QList<Plugin *> PanelPluginsModel::plugins() const
{
    QList<Plugin *> plugins;
    for (auto const & p : mPlugins)
        if (!p.second.isNull())
            plugins.append(p.second.data());
    return std::move(plugins);
}

Plugin* PanelPluginsModel::pluginByName(QString name) const
{
    for (auto const & p : mPlugins)
        if (p.first == name)
            return p.second.data();
    return nullptr;
}

Plugin* PanelPluginsModel::pluginByID(QString id) const
{
    for (auto const & p : mPlugins)
    {
        Plugin *plugin = p.second.data();
        if (plugin->desktopFile().id() == id)
            return plugin;
    }
    return nullptr;
}

void PanelPluginsModel::addPlugin(const LxQt::PluginInfo &desktopFile)
{
    Plugin *p = pluginByID(desktopFile.id());
    if (p && p->iPlugin()->flags().testFlag(ILxQtPanelPlugin::SingleInstance))
        return;

    QString name = findNewPluginSettingsGroup(desktopFile.id());

    QPointer<Plugin> plugin = loadPlugin(desktopFile, name);
    if (plugin.isNull())
        return;

    beginInsertRows(QModelIndex(), mPlugins.size(), mPlugins.size());
    mPlugins.append({name, plugin});
    endInsertRows();
    mPanel->settings()->setValue(mNamesKey, pluginNames());
    emit pluginAdded(plugin.data());
}

void PanelPluginsModel::removePlugin(pluginslist_t::iterator plugin)
{
    if (mPlugins.end() != plugin)
    {
        mPanel->settings()->remove(plugin->first);
        Plugin * p = plugin->second.data();
        const int row = plugin - mPlugins.begin();
        beginRemoveRows(QModelIndex(), row, row);
        mPlugins.erase(plugin);
        endRemoveRows();
        mActive = mPlugins.isEmpty() ? QModelIndex() : createIndex(mPlugins.size() > row ? row : row - 1, 0);
        emit pluginRemoved(p); // p can be nullptr
        mPanel->settings()->setValue(mNamesKey, pluginNames());
        if (nullptr != p)
            p->deleteLater();
    }
}

void PanelPluginsModel::removePlugin()
{
    Plugin * p = qobject_cast<Plugin*>(sender());
    auto plugin = std::find_if(mPlugins.begin(), mPlugins.end(),
                               [p] (pluginslist_t::const_reference obj) { return p == obj.second; });
    removePlugin(std::move(plugin));
}

void PanelPluginsModel::movePlugin(Plugin * plugin, QString const & nameAfter)
{
    //merge list of plugins (try to preserve original position)
    const int from =
        std::find_if(mPlugins.begin(), mPlugins.end(), [plugin] (pluginslist_t::const_reference obj) { return plugin == obj.second.data(); })
        - mPlugins.begin();
    const int to =
        std::find_if(mPlugins.begin(), mPlugins.end(), [nameAfter] (pluginslist_t::const_reference obj) { return nameAfter == obj.first; })
        - mPlugins.begin();
    const int to_plugins = from < to ? to - 1 : to;

    if (from != to && from != to_plugins)
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        mPlugins.move(from, to_plugins);
        endMoveRows();
        emit pluginMoved(plugin);
        mPanel->settings()->setValue(mNamesKey, pluginNames());
    }
}

void PanelPluginsModel::loadPlugins(QStringList const & desktopDirs)
{
    QStringList plugin_names = mPanel->settings()->value(mNamesKey).toStringList();

#ifdef DEBUG_PLUGIN_LOADTIME
    QElapsedTimer timer;
    timer.start();
    qint64 lastTime = 0;
#endif
    for (auto const & name : plugin_names)
    {
        pluginslist_t::iterator i = mPlugins.insert(mPlugins.end(), {name, nullptr});
        QString type = mPanel->settings()->value(name + "/type").toString();
        if (type.isEmpty())
        {
            qWarning() << QString("Section \"%1\" not found in %2.").arg(name, mPanel->settings()->fileName());
            continue;
        }

        LxQt::PluginInfoList list = LxQt::PluginInfo::search(desktopDirs, "LxQtPanel/Plugin", QString("%1.desktop").arg(type));
        if( !list.count())
        {
            qWarning() << QString("Plugin \"%1\" not found.").arg(type);
            continue;
        }

        i->second = loadPlugin(list.first(), name);
#ifdef DEBUG_PLUGIN_LOADTIME
        qDebug() << "load plugin" << type << "takes" << (timer.elapsed() - lastTime) << "ms";
        lastTime = timer.elapsed();
#endif
    }
}

QPointer<Plugin> PanelPluginsModel::loadPlugin(LxQt::PluginInfo const & desktopFile, QString const & settingsGroup)
{
    std::unique_ptr<Plugin> plugin(new Plugin(desktopFile, mPanel->settings()->fileName(), settingsGroup, mPanel));
    if (plugin->isLoaded())
    {
        connect(mPanel, &LxQtPanel::realigned, plugin.get(), &Plugin::realign);
        connect(plugin.get(), &Plugin::remove,
                this, static_cast<void (PanelPluginsModel::*)()>(&PanelPluginsModel::removePlugin));
        return plugin.release();
    }

    return nullptr;
}

QString PanelPluginsModel::findNewPluginSettingsGroup(const QString &pluginType) const
{
    QStringList groups = mPanel->settings()->childGroups();
    groups.sort();

    // Generate new section name
    for (int i = 2; true; ++i)
        if (!groups.contains(QStringLiteral("%1%2").arg(pluginType).arg(i)))
            return QStringLiteral("%1%2").arg(pluginType).arg(i);
}

void PanelPluginsModel::onActivatedIndex(QModelIndex const & index)
{
    mActive = index;
}

bool PanelPluginsModel::isActiveIndexValid() const
{
    return mActive.isValid() && QModelIndex() == mActive.parent()
        && 0 == mActive.column() && mPlugins.size() > mActive.row();
}

void PanelPluginsModel::onMovePluginUp()
{
    if (!isActiveIndexValid())
        return;

    const int row = mActive.row();
    if (0 >= row)
        return; //can't move up

    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);
    mPlugins.swap(row - 1, row);
    endMoveRows();
    pluginslist_t::const_reference moved_plugin = mPlugins[row - 1];
    pluginslist_t::const_reference prev_plugin = mPlugins[row];

    emit pluginMoved(moved_plugin.second.data());
    //emit signal for layout only in case both plugins are loaded/displayed
    if (!moved_plugin.second.isNull() && !prev_plugin.second.isNull())
        emit pluginMovedUp(moved_plugin.second.data());

    mPanel->settings()->setValue(mNamesKey, pluginNames());
}

void PanelPluginsModel::onMovePluginDown()
{
    if (!isActiveIndexValid())
        return;

    const int row = mActive.row();
    if (mPlugins.size() <= row + 1)
        return; //can't move down

    beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2);
    mPlugins.swap(row, row + 1);
    endMoveRows();
    pluginslist_t::const_reference moved_plugin = mPlugins[row + 1];
    pluginslist_t::const_reference next_plugin = mPlugins[row];

    emit pluginMoved(moved_plugin.second.data());
    //emit signal for layout only in case both plugins are loaded/displayed
    if (!moved_plugin.second.isNull() && !next_plugin.second.isNull())
        emit pluginMovedUp(next_plugin.second.data());

    mPanel->settings()->setValue(mNamesKey, pluginNames());
}

void PanelPluginsModel::onConfigurePlugin()
{
    if (!isActiveIndexValid())
        return;

    Plugin * const plugin = mPlugins[mActive.row()].second.data();
    if (nullptr != plugin && (ILxQtPanelPlugin::HaveConfigDialog & plugin->iPlugin()->flags()))
        plugin->showConfigureDialog();
}

void PanelPluginsModel::onRemovePlugin()
{
    if (!isActiveIndexValid())
        return;

    auto plugin = mPlugins.begin() + mActive.row();
    if (plugin->second.isNull())
        removePlugin(std::move(plugin));
    else
        plugin->second->requestRemove();
}
