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


#ifndef LXQTFANCYMENUWINDOW_H
#define LXQTFANCYMENUWINDOW_H

#include <QWidget>
#include <QTimer>

#include "lxqtfancymenutypes.h"

class QLineEdit;
class QToolButton;
class QListView;
class QModelIndex;

class QHBoxLayout;
class QVBoxLayout;

class XdgMenu;

class LXQtFancyMenuAppMap;
class LXQtFancyMenuAppModel;
class LXQtFancyMenuCategoriesModel;

class LXQtFancyMenuWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LXQtFancyMenuWindow(QWidget *parent = nullptr);
    ~LXQtFancyMenuWindow();

    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    bool rebuildMenu(const XdgMenu &menu);

    void setCurrentCategory(int cat);

    bool eventFilter(QObject *watched, QEvent *e) override;

    QStringList favorites() const;
    void setFavorites(const QStringList &newFavorites);

    void setFilterClear(bool newFilterClear);

    void setButtonPosition(LXQtFancyMenuButtonPosition pos);
    void setCategoryPosition(LXQtFancyMenuCategoryPosition pos);

    void updateButtonIconSize();

    void setSearchEditFocus();

    void setCustomFont(const QFont& f);

    void setAutoSelection(bool autoSel) {
        mAutoSel = autoSel;
        if (!mAutoSel)
            mSelTimer.stop();
    }
    void setAutoSelectionDelay(int delay) {
        mSelTimer.setInterval(delay);
    }

signals:
    void aboutToShow();
    void aboutToHide();
    void favoritesChanged();

public slots:
    void doSearch();
    void setSearchQuery(const QString& text);

protected:
    void hideEvent(QHideEvent *e);
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);

private slots:
    void activateCategory(const QModelIndex& idx);
    void activateAppAtIndex(const QModelIndex& idx);
    void activateCurrentApp();

    void runPowerDialog();
    void runSystemConfigDialog();

    void onAppViewCustomMenu(const QPoint &p);

    void autoSelect();

private:
    void runCommandHelper(const QString& cmd);

    void addToFavorites(const QString& desktopFile);
    void removeFromFavorites(const QString& desktopFile);

private:
    // Use 3:2 stretch factors so app view is slightly wider than category view
    static const int APP_VIEW_STRETCH = 3;
    static const int CAT_VIEW_STRETCH = 2;

    QStringList mFavorites;

    QVBoxLayout *mMainLayout;
    QHBoxLayout *mButtonsLayout;
    QHBoxLayout *mViewLayout;

    QToolButton *mSettingsButton;
    QToolButton *mPowerButton;
    QLineEdit *mSearchEdit;
    QListView *mAppView;
    QListView *mCategoryView;

    LXQtFancyMenuAppMap *mAppMap;
    LXQtFancyMenuAppModel *mAppModel;
    LXQtFancyMenuCategoriesModel *mCategoryModel;

    QTimer mSearchTimer;
    QTimer mSelTimer;
    bool mAutoSel = false;
    bool mFilterClear = false;
};

#endif // LXQTFANCYMENUWINDOW_H
