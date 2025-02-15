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


#ifndef LXQTFANCYMENUAPPMAP_H
#define LXQTFANCYMENUAPPMAP_H

#include <QMap>
#include <QList>
#include <QStringList>
#include <QIcon>

#include <XdgDesktopFile>

#include "lxqtfancymenutypes.h"

class XdgMenu;
class QDomElement;

struct LXQtFancyMenuAppItem
{
    QString desktopFile;
    QString title;
    QString comment;
    QStringList keywords;
    QStringList exec;
    QIcon icon;
    XdgDesktopFile desktopFileCache;
};

class LXQtFancyMenuAppMap
{
public:
    enum SpecialCategory
    {
        FavoritesCategory = 0,
        AllAppsCategory = 1
    };

    typedef LXQtFancyMenuAppItem AppItem;

    struct Category
    {
        QString menuName;
        QString menuTitle;
        QIcon icon;

        struct Item
        {
            AppItem *appItem = nullptr;
            LXQtFancyMenuItemType type = LXQtFancyMenuItemType::AppItem;
        };

        QList<Item> apps;
        LXQtFancyMenuItemType type;
    };

    LXQtFancyMenuAppMap();
    ~LXQtFancyMenuAppMap();

    void clear();
    void clearFavorites();
    bool rebuildModel(const XdgMenu &menu);

    void setFavorites(const QStringList& favorites);
    QStringList getFavorites() const;

    int getFavoriteIndex(const QString& desktopFile) const;

    inline int getFavoriteCount() const { return mCategories[0].apps.count(); }

    inline bool isFavorite(const QString& desktopFile) const
    {
        return getFavoriteIndex(desktopFile) != -1;
    }

    void addToFavorites(const QString& desktopFile);
    void removeFromFavorites(const QString& desktopFile);
    void moveFavoriteItem(int oldPos, int newPos);

    inline int getCategoriesCount() const { return mCategories.size(); }
    inline const Category& getCategoryAt(int index) { return mCategories.at(index); }

    inline int getTotalAppCount() const { return mAppSortedByName.size(); }

    AppItem *getAppAt(int index);

    QList<const AppItem *> getMatchingApps(const QString& query) const;

private:
    void parseMenu(const QDomElement& menu, const QString &topLevelCategory);
    void parseAppLink(const QDomElement& app, const QString &topLevelCategory);
    void parseSeparator(const QDomElement &sep, const QString &topLevelCategory);

    AppItem *loadAppItem(const QString& desktopFile);

private:
    typedef QMap<QString, AppItem *> AppMap;
    AppMap mAppSortedByDesktopFile;
    AppMap mAppSortedByName;
    QList<Category> mCategories;

    // Cache sort by name map access
    AppMap::const_iterator mCachedIterator;
    int mCachedIndex;
};

#endif // LXQTFANCYMENUAPPMAP_H
