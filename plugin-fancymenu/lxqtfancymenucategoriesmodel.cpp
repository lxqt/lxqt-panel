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


#include "lxqtfancymenucategoriesmodel.h"
#include "lxqtfancymenuappmap.h"

LXQtFancyMenuCategoriesModel::LXQtFancyMenuCategoriesModel(QObject *parent)
    : QAbstractListModel(parent)
    , mAppMap(nullptr)
{
}

int LXQtFancyMenuCategoriesModel::rowCount(const QModelIndex &p) const
{
    if(!mAppMap || p.isValid())
        return 0;

    return mAppMap->getCategoriesCount();
}

QVariant LXQtFancyMenuCategoriesModel::data(const QModelIndex &idx, int role) const
{
    if (!mAppMap || !idx.isValid() || idx.row() >= mAppMap->getCategoriesCount())
        return QVariant();

    const LXQtFancyMenuAppMap::Category& item = mAppMap->getCategoryAt(idx.row());

    switch (role)
    {
    case Qt::DisplayRole:
        return item.menuTitle;
    case Qt::EditRole:
        return item.menuName;
    case Qt::DecorationRole:
        return item.icon;
    case LXQtFancyMenuItemIsSeparatorRole:
        if(item.type == LXQtFancyMenuItemType::SeparatorItem)
            return 1;
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags LXQtFancyMenuCategoriesModel::flags(const QModelIndex &idx) const
{
    if (!mAppMap || !idx.isValid() || idx.row() >= mAppMap->getCategoriesCount())
        return Qt::NoItemFlags;

    const LXQtFancyMenuAppMap::Category& item = mAppMap->getCategoryAt(idx.row());
    if(item.type == LXQtFancyMenuItemType::SeparatorItem)
        return Qt::NoItemFlags;

    return QAbstractListModel::flags(idx);
}

void LXQtFancyMenuCategoriesModel::reloadAppMap(bool end)
{
    if(!end)
        beginResetModel();
    else
        endResetModel();
}

LXQtFancyMenuAppMap *LXQtFancyMenuCategoriesModel::appMap() const
{
    return mAppMap;
}

void LXQtFancyMenuCategoriesModel::setAppMap(LXQtFancyMenuAppMap *newAppMap)
{
    mAppMap = newAppMap;
}
