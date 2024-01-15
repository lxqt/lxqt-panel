/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2023 LXQt team
 * Authors:
 *  Filippo Gentile <filippogentile@disroot.org>
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


#ifndef LXQTFANCYMENUCATEGORIESMODEL_H
#define LXQTFANCYMENUCATEGORIESMODEL_H

#include <QAbstractListModel>

class LXQtFancyMenuAppMap;

class LXQtFancyMenuCategoriesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LXQtFancyMenuCategoriesModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &p = QModelIndex()) const override;

    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const override;

    // Separator support:
    Qt::ItemFlags flags(const QModelIndex &idx) const override;

    void reloadAppMap(bool end);

    LXQtFancyMenuAppMap *appMap() const;
    void setAppMap(LXQtFancyMenuAppMap *newAppMap);

private:
    LXQtFancyMenuAppMap *mAppMap;
};

#endif // LXQTFANCYMENUCATEGORIESMODEL_H
