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


#ifndef LXQTFANCYMENUTYPES_H
#define LXQTFANCYMENUTYPES_H

#include <qnamespace.h>

enum LXQtFancyMenuButtonPosition : bool
{
    Bottom = 0,
    Top = 1
};

enum LXQtFancyMenuCategoryPosition : bool
{
    Left = 0,
    Right = 1
};

enum class LXQtFancyMenuItemType
{
    AppItem = 0,
    CategoryItem,
    SeparatorItem
};

static constexpr const int LXQtFancyMenuItemIsSeparatorRole = Qt::UserRole + 1;

static constexpr const QLatin1String LXQtMenuFile("lxqt-applications.menu");

#endif // LXQTFANCYMENUTYPES_H
