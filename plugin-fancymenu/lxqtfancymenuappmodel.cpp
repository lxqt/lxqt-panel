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


#include "lxqtfancymenuappmodel.h"
#include "lxqtfancymenuappmap.h"

#include <QMimeData>
#include <QUrl>

static const QLatin1String FavoritesDragMimeType("application/x-lxqtfavoritesdragrow");

LXQtFancyMenuAppModel::LXQtFancyMenuAppModel(QObject *parent)
    : QAbstractListModel(parent)
    , mAppMap(nullptr)
    , mCurrentCategory(0)
    , mInSearch(false)
{
}

int LXQtFancyMenuAppModel::rowCount(const QModelIndex &p) const
{
    if(!mAppMap || p.isValid() || mCurrentCategory < 0 || mCurrentCategory >= mAppMap->getCategoriesCount())
        return 0;

    if(mInSearch)
        return mSearchMatches.size();

    if(mCurrentCategory == LXQtFancyMenuAppMap::AllAppsCategory)
        return mAppMap->getTotalAppCount(); //Special "All Applications" category

    return mAppMap->getCategoryAt(mCurrentCategory).apps.size();
}

QVariant LXQtFancyMenuAppModel::data(const QModelIndex &idx, int role) const
{
    if(!idx.isValid())
        return QVariant();

    const LXQtFancyMenuAppMap::AppItem* item = getAppAt(idx.row());
    LXQtFancyMenuItemType type = getItemTypeAt(idx.row());
    if(!item && type == LXQtFancyMenuItemType::AppItem)
        return QVariant();

    if(!item)
    {
        if(role == LXQtFancyMenuItemIsSeparatorRole)
            return 1;
        return QVariant();
    }

    switch (role)
    {
    case Qt::DisplayRole:
        return item->title;
    case Qt::EditRole:
        return item->desktopFile;
    case Qt::DecorationRole:
        return item->icon;
    case Qt::ToolTipRole:
    {
        return item->comment;
    }
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags LXQtFancyMenuAppModel::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags f = QAbstractListModel::flags(idx);

    if(mCurrentCategory == LXQtFancyMenuAppMap::FavoritesCategory)
        f.setFlag(Qt::ItemIsDropEnabled); //Allow drag-drop of favorites

    const LXQtFancyMenuAppMap::AppItem* item = getAppAt(idx.row());
    LXQtFancyMenuItemType type = getItemTypeAt(idx.row());
    if(!item || type == LXQtFancyMenuItemType::SeparatorItem)
        return f;

    if (idx.isValid())
        f.setFlag(Qt::ItemIsDragEnabled);
    return f;
}

QStringList LXQtFancyMenuAppModel::mimeTypes() const
{
    return {FavoritesDragMimeType};
}

QMimeData *LXQtFancyMenuAppModel::mimeData(const QModelIndexList &indexes) const
{
    QList<QUrl> urls;

    int row = -1;
    for(const QModelIndex& idx : indexes)
    {
        const LXQtFancyMenuAppMap::AppItem* item = getAppAt(idx.row());
        if(!item)
            continue;
        urls << QUrl::fromLocalFile(item->desktopFile);
        if(row == -1)
            row = idx.row();
    }

    QMimeData *mimeData = new QMimeData();
    mimeData->setUrls(urls);
    if(row != -1)
        mimeData->setData(FavoritesDragMimeType, QByteArray::number(row));
    return mimeData;
}

bool LXQtFancyMenuAppModel::dropMimeData(const QMimeData *data_, Qt::DropAction /*action*/,
                                         int row, int /*column*/, const QModelIndex &p)
{
    if(mCurrentCategory != LXQtFancyMenuAppMap::FavoritesCategory)
        return false;

    auto urls = data_->urls();
    if(urls.isEmpty())
        return false;

    QString desktopFile =urls.first().toLocalFile();

    int oldRow = mAppMap->getFavoriteIndex(desktopFile);
    if(oldRow == -1)
        return false;

    if(row == -1 && p.isValid())
    {
        // Dropped onto item but this model is a flat list
        // If going upwards we drop above destination item
        row = p.row();
        if(oldRow < p.row())
        {
            // If going downwards we drop below destination item
            row++;
        }
    }

    if(row == -1)
        return false;

    // Compensate the fact that we first remove the item
    // so all indexes are shifted by -1, store original value
    int realRow = row;
    if(row > oldRow)
        row--;

    if(row == oldRow)
        return false; // No-op

    // realRow is needed because beginMoveRows() behaves differenlty than
    // QList<...>::move() on index counting.
    beginMoveRows(QModelIndex(), oldRow, oldRow, QModelIndex(), realRow);

    mAppMap->moveFavoriteItem(oldRow, row);

    endMoveRows();

    emit favoritesChanged();

    return true;
}

Qt::DropActions LXQtFancyMenuAppModel::supportedDragActions() const
{
    return Qt::CopyAction | Qt::LinkAction | Qt::MoveAction;
}

void LXQtFancyMenuAppModel::reloadAppMap(bool end)
{
    if(!end)
        beginResetModel();
    else
        endResetModel();
}

void LXQtFancyMenuAppModel::setCurrentCategory(int category)
{
    beginResetModel();
    mCurrentCategory = category;
    endResetModel();
}

void LXQtFancyMenuAppModel::showSearchResults(const QList<const LXQtFancyMenuAppItem *> &matches)
{
    beginResetModel();
    mSearchMatches = matches;
    mInSearch = true;
    endResetModel();
}

void LXQtFancyMenuAppModel::endSearch()
{
    beginResetModel();
    mSearchMatches.clear();
    mSearchMatches.squeeze();
    mInSearch = false;
    endResetModel();
}

LXQtFancyMenuAppMap *LXQtFancyMenuAppModel::appMap() const
{
    return mAppMap;
}

void LXQtFancyMenuAppModel::setAppMap(LXQtFancyMenuAppMap *newAppMap)
{
    mAppMap = newAppMap;
}

const LXQtFancyMenuAppItem *LXQtFancyMenuAppModel::getAppAt(int idx) const
{
    if(!mAppMap || idx < 0 || mCurrentCategory < 0 || mCurrentCategory >= mAppMap->getCategoriesCount())
        return nullptr;

    if(mInSearch)
        return mSearchMatches.value(idx, nullptr);

    if(mCurrentCategory == LXQtFancyMenuAppMap::AllAppsCategory)
        return mAppMap->getAppAt(idx); //Special "All Applications" category

    const LXQtFancyMenuAppMap::Category& cat = mAppMap->getCategoryAt(mCurrentCategory);
    if(idx >= cat.apps.size())
        return nullptr;

    const LXQtFancyMenuAppMap::Category::Item& item = cat.apps.at(idx);
    return item.appItem;
}

LXQtFancyMenuItemType LXQtFancyMenuAppModel::getItemTypeAt(int idx) const
{
    if(!mAppMap || idx < 0 || mCurrentCategory < 0 || mCurrentCategory >= mAppMap->getCategoriesCount())
        return LXQtFancyMenuItemType::AppItem;

    if(mInSearch)
        return LXQtFancyMenuItemType::AppItem;

    if(mCurrentCategory == LXQtFancyMenuAppMap::AllAppsCategory)
        return LXQtFancyMenuItemType::AppItem; //Special "All Applications" category

    const LXQtFancyMenuAppMap::Category& cat = mAppMap->getCategoryAt(mCurrentCategory);
    if(idx >= cat.apps.size())
        return LXQtFancyMenuItemType::AppItem;

    const LXQtFancyMenuAppMap::Category::Item& item = cat.apps.at(idx);
    return item.type;
}

bool LXQtFancyMenuAppModel::isInSearch() const
{
    return mInSearch;
}
