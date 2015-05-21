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

#ifndef PANELPLUGINSMODEL_H
#define PANELPLUGINSMODEL_H

#include <QAbstractListModel>
#include <memory>

namespace LxQt
{
    class PluginInfo;
    struct PluginData;
}

class LxQtPanel;
class Plugin;

class PanelPluginsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PanelPluginsModel(LxQtPanel * panel,
                      QString const & namesKey,
                      QStringList const & desktopDirs,
                      QObject * parent = nullptr);
    ~PanelPluginsModel();

    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;

    QStringList pluginNames() const;
    QList<Plugin *> plugins() const;
    Plugin *pluginByName(QString name) const;
    Plugin *pluginByID(QString id) const;

    /*!
     * \param plugin plugin that has been moved
     * \param nameAfter name of plugin that is right after moved plugin
     */
    void movePlugin(Plugin * plugin, QString const & nameAfter);

signals:
    void pluginAdded(Plugin * plugin);
    void pluginRemoved(Plugin * plugin);
    void pluginMoved(Plugin * plugin); //plugin can be nullptr in case of move of not loaded plugin
    /*!
     * Emiting only move-up for simplification of using (and problematic layout/list move)
     */
    void pluginMovedUp(Plugin * plugin);

public slots:
    void addPlugin(const LxQt::PluginInfo &desktopFile);
    void removePlugin();

    // slots for configuration dialog
    void onActivatedIndex(QModelIndex const & index);
    void onMovePluginUp();
    void onMovePluginDown();
    void onConfigurePlugin();
    void onRemovePlugin();

private:
    typedef QList<QPair <QString/*name*/, QPointer<Plugin> > > pluginslist_t;

private:
    void loadPlugins(QStringList const & desktopDirs);
    QPointer<Plugin> loadPlugin(LxQt::PluginInfo const & desktopFile, QString const & settingsGroup);
    QString findNewPluginSettingsGroup(const QString &pluginType) const;
    bool isActiveIndexValid() const;
    void removePlugin(pluginslist_t::iterator plugin);

    const QString mNamesKey;
    pluginslist_t mPlugins;
    LxQtPanel * mPanel;
    QPersistentModelIndex mActive;
};

Q_DECLARE_METATYPE(Plugin const *)

#endif // PANELPLUGINSMODEL_H
