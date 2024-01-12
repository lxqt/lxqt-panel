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


#include "lxqtfancymenuappmap.h"

#include <XdgMenu>
#include <XdgIcon>

#include <QCoreApplication>

class LXQtFancyMenuAppMapStrings
{
    Q_DECLARE_TR_FUNCTIONS(LXQtFancyMenuAppMapStrings)
};


LXQtFancyMenuAppMap::LXQtFancyMenuAppMap()
{
    mCachedIndex = -1;
    mCachedIterator = mAppSortedByName.constEnd();

    //Add Favorites category
    Category favorites;
    favorites.menuTitle = LXQtFancyMenuAppMapStrings::tr("Favorites");
    favorites.icon = XdgIcon::fromTheme(QLatin1String("bookmarks"));
    mCategories.append(favorites);

    //Add All Apps category
    Category allAppsCategory;
    allAppsCategory.menuTitle = LXQtFancyMenuAppMapStrings::tr("All Applications");
    allAppsCategory.icon = XdgIcon::fromTheme(QLatin1String("folder"));
    mCategories.append(allAppsCategory);

    //Add separator
    Category sepatorCategory;
    sepatorCategory.type = LXQtFancyMenuItemType::SeparatorItem;
    mCategories.append(sepatorCategory);
}

LXQtFancyMenuAppMap::~LXQtFancyMenuAppMap()
{
    clear();
    clearFavorites();
}

void LXQtFancyMenuAppMap::clear()
{
    // Keep Favorites, All Applications and separator
    mCategories.erase(mCategories.begin() + 3, mCategories.end());

    mAppSortedByName.clear();
    qDeleteAll(mAppSortedByDesktopFile);
    mAppSortedByDesktopFile.clear();

    mCachedIndex = -1;
    mCachedIterator = mAppSortedByName.constEnd();
}

void LXQtFancyMenuAppMap::clearFavorites()
{
    Category& favoritesCatRef = mCategories[0];
    for(Category::Item& item : favoritesCatRef.apps)
    {
        if(item.appItem)
        {
            delete item.appItem;
            item.appItem = nullptr;
        }
    }
    favoritesCatRef.apps.clear();
}

bool LXQtFancyMenuAppMap::rebuildModel(const XdgMenu &menu)
{
    clear();

    QDomElement rootMenu = menu.xml().documentElement();
    parseMenu(rootMenu, QString());

    mCategories.squeeze();

    return true;
}

void LXQtFancyMenuAppMap::setFavorites(const QStringList &favorites)
{
    clearFavorites();

    Category& favoritesCatRef = mCategories[0];

    for(const QString& desktopFile : favorites)
    {
        Category::Item item;
        item.type = LXQtFancyMenuItemType::AppItem;
        item.appItem = loadAppItem(desktopFile);
        if(!item.appItem)
            continue;
        favoritesCatRef.apps.append(item);
    }
}

bool LXQtFancyMenuAppMap::isFavorite(const QString &desktopFile) const
{
    const Category& favoritesCat = mCategories.at(0);
    for(const Category::Item& item : favoritesCat.apps)
    {
        if(item.appItem && item.appItem->desktopFile == desktopFile)
            return true;
    }

    return false;
}

void LXQtFancyMenuAppMap::addToFavorites(const QString &desktopFile)
{
    if(isFavorite(desktopFile))
        return;

    Category::Item item;
    item.type = LXQtFancyMenuItemType::AppItem;
    item.appItem = loadAppItem(desktopFile);
    if(!item.appItem)
        return;

    Category& favoritesCatRef = mCategories[0];
    favoritesCatRef.apps.append(item);
}

void LXQtFancyMenuAppMap::removeFromFavorites(const QString &desktopFile)
{
    if(!isFavorite(desktopFile))
        return;

    Category& favoritesCatRef = mCategories[0];
    for(auto it = favoritesCatRef.apps.begin(); it != favoritesCatRef.apps.end(); it++)
    {
        AppItem *appItem = (*it).appItem;
        if(appItem && appItem->desktopFile == desktopFile)
        {
            favoritesCatRef.apps.erase(it);
            delete appItem;
            return;
        }
    }
}

LXQtFancyMenuAppMap::AppItem *LXQtFancyMenuAppMap::getAppAt(int index)
{
    if(index < 0 || index >= getTotalAppCount())
        return nullptr;

    if(mCachedIndex != -1)
    {
        if(index == mCachedIndex + 1)
        {
            //Fast case, go to next row
            mCachedIndex++;
            mCachedIterator++;
        }

        if(index == mCachedIndex)
            return *mCachedIterator;

        int dist1 = qAbs(mCachedIndex - index);
        if(dist1 < index)
        {
            std::advance(mCachedIterator, index - mCachedIndex);
            mCachedIndex = index;
            return *mCachedIterator;
        }
    }

    // Recalculate cached iterator
    mCachedIterator = mAppSortedByName.constBegin();
    std::advance(mCachedIterator, index);
    mCachedIndex = index;
    return *mCachedIterator;
}

QVector<const LXQtFancyMenuAppMap::AppItem *> LXQtFancyMenuAppMap::getMatchingApps(const QString &query) const
{
    QVector<const AppItem *> byName;
    QVector<const AppItem *> byKeyword;

    //TODO: implement some kind of score to get better matches on top

    for(const AppItem *app : qAsConst(mAppSortedByName))
    {
        if(app->title.contains(query))
        {
            byName.append(app);
            continue;
        }

        if(app->comment.contains(query))
        {
            byKeyword.append(app);
            continue;
        }

        for(const QString& key : app->keywords)
        {
            if(key.startsWith(query))
            {
                byKeyword.append(app);
                break;
            }
        }
    }

    // Give priority to title matches
    byName += byKeyword;

    return byName;
}

void LXQtFancyMenuAppMap::parseMenu(const QDomElement &menu, const QString& topLevelCategory)
{
    QDomElement e = menu.firstChildElement();
    while(!e.isNull())
    {
        if(e.tagName() == QLatin1String("Menu"))
        {
            if(topLevelCategory.isEmpty())
            {
                //This is a top level menu
                Category item;
                item.type = LXQtFancyMenuItemType::CategoryItem;
                item.menuName = e.attribute(QLatin1String("name"));
                item.menuTitle = e.attribute(QLatin1Literal("title"), item.menuName);
                QString iconName = e.attribute(QLatin1String("icon"));
                item.icon = XdgIcon::fromTheme(iconName);
                mCategories.append(item);

                //Merge sub menu to parent
                parseMenu(e, item.menuName);
            }
            else
            {
                //Merge sub menu to parent
                parseMenu(e, topLevelCategory);
            }
        }
        else if(!topLevelCategory.isEmpty())
        {
            if(e.tagName() == QLatin1String("AppLink"))
                parseAppLink(e, topLevelCategory);
            else if(e.tagName() == QLatin1String("Separator"))
                parseSeparator(e, topLevelCategory);
        }

        e = e.nextSiblingElement();
    }
}

void LXQtFancyMenuAppMap::parseAppLink(const QDomElement &app, const QString& topLevelCategory)
{
    QString desktopFile = app.attribute(QLatin1String("desktopFile"));

    // Check if already added
    AppItem *appItem = mAppSortedByDesktopFile.value(desktopFile, nullptr);
    if(!appItem)
    {
        // Add new app
        appItem = loadAppItem(desktopFile);
        if(!appItem)
            return; // Invalid app

        mAppSortedByDesktopFile.insert(appItem->desktopFile, appItem);
        mAppSortedByName.insert(appItem->title, appItem);
    }

    // Now add app to category
    for(Category &category : mCategories)
    {
        if(category.menuName == topLevelCategory)
        {
            Category::Item item;
            item.appItem = appItem;
            item.type = LXQtFancyMenuItemType::AppItem;
            category.apps.append(item);
            break;
        }
    }
}

void LXQtFancyMenuAppMap::parseSeparator(const QDomElement &sep, const QString& topLevelCategory)
{
    Q_UNUSED(sep)

    // Find category
    for(Category &category : mCategories)
    {
        if(category.menuName != topLevelCategory)
            continue;

        // XdgMenu already cares of removing consecutive separators
        // Or separators put as first or last items
        Category::Item item;
        item.type = LXQtFancyMenuItemType::SeparatorItem;
        category.apps.append(item);

        break;
    }
}

LXQtFancyMenuAppMap::AppItem *LXQtFancyMenuAppMap::loadAppItem(const QString &desktopFile)
{
    XdgDesktopFile f;
    if(!f.load(desktopFile))
        return nullptr; // Invalid App

    AppItem *item = new AppItem;
    item->desktopFile = desktopFile;
    item->title = f.name();
    item->comment = f.comment();
    if(item->comment.isEmpty())
        item->comment = f.localizedValue(QLatin1String("GenericName")).toString();
    item->icon = f.icon();
    item->desktopFileCache = f;

    item->keywords << f.localizedValue(QLatin1String("Keywords")).toString().toLower().split(QLatin1Char(';'));
    item->keywords.append(item->title.toLower().split(QLatin1Char(' ')));
    item->keywords.append(item->comment.toLower().split(QLatin1Char(' ')));
    return item;
}
