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


#include "lxqtfancymenuwindow.h"

#include "lxqtfancymenuappmap.h"
#include "lxqtfancymenuappmodel.h"
#include "lxqtfancymenucategoriesmodel.h"

#include <QLineEdit>
#include <QToolButton>
#include <QListView>
#include <QPainter>
#include <QMenu>
#include <QWindow>
#include <QScreen>
#include <QStandardPaths>
#include <QDir>
#include <QMimeData>
#include <XdgIcon>
#include <QFile>

#include <QApplication>
#include <QClipboard>

#include <QBoxLayout>

#include <QMessageBox>

#include <QProcess>

#include <QKeyEvent>
#include <QCoreApplication>

#include <QProxyStyle>
#include <QStyledItemDelegate>

namespace
{
class SingleActivateStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;
    int styleHint(StyleHint hint, const QStyleOption * option = nullptr, const QWidget * widget = nullptr, QStyleHintReturn * returnData = nullptr) const override
    {
        if(hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
            return 1;
        return QProxyStyle::styleHint(hint, option, widget, returnData);

    }
};

class SeparatorDelegate : public QStyledItemDelegate
{
public:
    SeparatorDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

    static bool isSeparator(const QModelIndex &index)
    {
        return index.data(LXQtFancyMenuItemIsSeparatorRole).toInt() == 1;
    }

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        if (isSeparator(index))
        {
            QRect rect = option.rect;
            if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView*>(option.widget))
                rect.setWidth(view->viewport()->width());
            const int margin = 6;
            painter->save();
            painter->setOpacity(0.4);
            painter->setPen(QPen(Qt::black));
            painter->drawLine(rect.topLeft().x() + margin ,
                              rect.topLeft().y(),
                              rect.topRight().x() - margin,
                              rect.topRight().y());
            painter->setPen(QPen(Qt::white));
            painter->drawLine(rect.topLeft().x() + margin ,
                              rect.topLeft().y() + 1,
                              rect.topRight().x() - margin,
                              rect.topRight().y() + 1);
            painter->restore();
        }
        else
        {
            QStyledItemDelegate::paint(painter, option, index);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override
    {
        if (isSeparator(index))
        {
            return QSize(2, 2);
        }

        return QStyledItemDelegate::sizeHint(option, index);
    }
};

}

LXQtFancyMenuWindow::LXQtFancyMenuWindow(QWidget *parent)
    : QWidget{parent, Qt::Popup}
{
    // Under some Wayland compositors, setting window flags in the c-tor of the base class
    // may not be enough for a correct positioning of the popup.
    setWindowFlags(Qt::Popup);

    SingleActivateStyle *s = new SingleActivateStyle;
    s->setParent(this);
    setStyle(s);

    mSearchTimer.setSingleShot(true);
    connect(&mSearchTimer, &QTimer::timeout, this, &LXQtFancyMenuWindow::doSearch);
    mSearchTimer.setInterval(350); // typing speed (not very fast)

    mAutoSelTimer.setSingleShot(true);
    connect(&mAutoSelTimer, &QTimer::timeout, this, &LXQtFancyMenuWindow::autoSelect);

    mSearchEdit = new QLineEdit;
    mSearchEdit->setPlaceholderText(tr("Search..."));
    mSearchEdit->setClearButtonEnabled(true);
    connect(mSearchEdit, &QLineEdit::textEdited, &mSearchTimer, qOverload<>(&QTimer::start));
    connect(mSearchEdit, &QLineEdit::returnPressed, this, &LXQtFancyMenuWindow::activateCurrentApp);

    mSettingsButton = new QToolButton;
    mSettingsButton->setIcon(XdgIcon::fromTheme(QStringLiteral("preferences-desktop"))); //TODO: preferences-system?
    mSettingsButton->setText(tr("Settings"));
    mSettingsButton->setToolTip(mSettingsButton->text());
    connect(mSettingsButton, &QToolButton::clicked, this, &LXQtFancyMenuWindow::runSystemConfigDialog);

    mPowerButton = new QToolButton;
    mPowerButton->setIcon(XdgIcon::fromTheme(QStringLiteral("system-shutdown")));
    mPowerButton->setText(tr("Leave"));
    mPowerButton->setToolTip(mPowerButton->text());
    connect(mPowerButton, &QToolButton::clicked, this, &LXQtFancyMenuWindow::runPowerDialog);

    mAppView = new QListView;
    mAppView->setObjectName(QStringLiteral("AppView"));
    mAppView->setSelectionMode(QListView::SingleSelection);
    mAppView->setDragEnabled(true);
    mAppView->setContextMenuPolicy(Qt::CustomContextMenu);
    mAppView->setItemDelegate(new SeparatorDelegate(this));

    mCategoryView = new QListView;
    mCategoryView->setObjectName(QStringLiteral("CategoryView"));
    mCategoryView->setSelectionMode(QListView::SingleSelection);
    mCategoryView->setItemDelegate(new SeparatorDelegate(this));

    // Meld category view with whole popup window
    // So remove its frame and do not auto-fill its background
    mCategoryView->setFrameShape(QFrame::NoFrame);
    mCategoryView->viewport()->setAutoFillBackground(false);

    mAppMap = new LXQtFancyMenuAppMap;

    mAppModel = new LXQtFancyMenuAppModel(this);
    mAppModel->setAppMap(mAppMap);
    mAppView->setModel(mAppModel);

    mCategoryModel = new LXQtFancyMenuCategoriesModel(this);
    mCategoryModel->setAppMap(mAppMap);
    mCategoryView->setModel(mCategoryModel);

    connect(mAppView, &QListView::activated, this, &LXQtFancyMenuWindow::activateAppAtIndex);
    connect(mAppView, &QListView::customContextMenuRequested, this, &LXQtFancyMenuWindow::onAppViewCustomMenu);
    connect(mCategoryView, &QListView::activated, this, &LXQtFancyMenuWindow::activateCategory);

    mMainLayout = new QVBoxLayout(this);

    mMainLayout->addWidget(mSearchEdit);

    mViewLayout = new QHBoxLayout;
    mViewLayout->addWidget(mAppView, APP_VIEW_STRETCH);
    mViewLayout->addWidget(mCategoryView, CAT_VIEW_STRETCH);
    mMainLayout->addLayout(mViewLayout);

    mButtonsLayout = new QHBoxLayout;
    mButtonsLayout->addStretch();
    mButtonsLayout->addWidget(mSettingsButton);
    mButtonsLayout->addWidget(mPowerButton);
    mMainLayout->addLayout(mButtonsLayout);

    updateButtonIconSize();

    setMinimumHeight(500);

    // Ensure all key presses go to search box
    setFocusProxy(mSearchEdit);
    mAppView->setFocusProxy(mSearchEdit);
    mCategoryView->setFocusProxy(mSearchEdit);

    // for auto-selection
    mAppView->viewport()->setMouseTracking(true);
    mAppView->viewport()->installEventFilter(this);
    mCategoryView->viewport()->setMouseTracking(true);
    mCategoryView->viewport()->installEventFilter(this);

    // Filter navigation keys
    mSearchEdit->installEventFilter(this);
}

LXQtFancyMenuWindow::~LXQtFancyMenuWindow()
{
    mAppModel->setAppMap(nullptr);
    mCategoryModel->setAppMap(nullptr);
    delete mAppMap;
    mAppMap = nullptr;
}

QSize LXQtFancyMenuWindow::sizeHint() const
{
    return size().expandedTo(minimumSizeHint());
}

QSize LXQtFancyMenuWindow::minimumSizeHint() const
{
    return QSize(450, 550);
}

bool LXQtFancyMenuWindow::rebuildMenu(const XdgMenu &menu)
{
    mAppModel->reloadAppMap(false);
    mCategoryModel->reloadAppMap(false);
    mAppMap->rebuildModel(menu);
    mAppModel->reloadAppMap(true);
    mCategoryModel->reloadAppMap(true);

    setCurrentCategory(LXQtFancyMenuAppMap::FavoritesCategory);

    return true;
}

void LXQtFancyMenuWindow::activateCategory(const QModelIndex &idx)
{
    setCurrentCategory(idx.row());
}

void LXQtFancyMenuWindow::activateAppAtIndex(const QModelIndex &idx)
{
    if(!idx.isValid())
        return;

    auto *app = mAppModel->getAppAt(idx.row());
    if(!app)
        return;

    app->desktopFileCache.startDetached();
    hide();
}

void LXQtFancyMenuWindow::activateCurrentApp()
{
    activateAppAtIndex(mAppView->currentIndex());
}

void LXQtFancyMenuWindow::runPowerDialog()
{
    runCommandHelper(QLatin1String("lxqt-leave"));
}

void LXQtFancyMenuWindow::runSystemConfigDialog()
{
    runCommandHelper(QLatin1String("lxqt-config"));
}

void LXQtFancyMenuWindow::onAppViewCustomMenu(const QPoint& p)
{
    QModelIndex idx = mAppView->indexAt(p);
    auto item = mAppModel->getAppAt(idx.row());
    if(!item)
        return;

    XdgDesktopFile df = item->desktopFileCache;
    QString file = df.fileName();

    QMenu menu;
    QAction *a;

    if (df.actions().count() > 0 && df.type() == XdgDesktopFile::Type::ApplicationType)
    {
        for (int i = 0; i < df.actions().count(); ++i)
        {
            QString actionString(df.actions().at(i));
            a = menu.addAction(df.actionIcon(actionString), df.actionName(actionString));
            connect(a, &QAction::triggered, this, [this, df, actionString] {
                df.actionActivate(actionString, QStringList());
                hide();
            });
        }
        menu.addSeparator();
    }

    a = menu.addAction(XdgIcon::fromTheme(QLatin1String("desktop")), tr("Add to desktop"));
    connect(a, &QAction::triggered, [file] {
        QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        QString desktopFile = desktop + QStringLiteral("/") + file.section(QStringLiteral("/"), -1);
        if (QFile::exists(desktopFile))
        {
            QMessageBox::StandardButton btn =
                QMessageBox::question(nullptr,
                                      tr("Question"),
                                      tr("A file with the same name already exists.\nDo you want to overwrite it?"));
            if (btn == QMessageBox::No)
                return;
            if (!QFile::remove(desktopFile))
            {
                QMessageBox::warning(nullptr,
                                     tr("Warning"),
                                     tr("The file cannot be overwritten."));
                return;
            }
        }
        QFile::copy(file, desktopFile);
    });

    a = menu.addAction(XdgIcon::fromTheme(QLatin1String("edit-copy")), tr("Copy"));
    connect(a, &QAction::triggered, this, [file] {
        QClipboard* clipboard = QApplication::clipboard();
        QMimeData* data = new QMimeData();
        data->setUrls({QUrl::fromLocalFile(file)});
        clipboard->setMimeData(data);
    });

    menu.addSeparator();

    QString canonicalFile = QDir(file).canonicalPath();
    if(mAppMap->isFavorite(canonicalFile))
    {
        a = menu.addAction(XdgIcon::fromTheme(QLatin1String("bookmark-remove")), tr("Remove from Favorites"));
        connect(a, &QAction::triggered, this, [this, canonicalFile] {
            removeFromFavorites(canonicalFile);
        });
    }
    else
    {
        a = menu.addAction(XdgIcon::fromTheme(QLatin1String("bookmark-new")), tr("Add to Favorites"));
        connect(a, &QAction::triggered, this, [this, canonicalFile] {
            addToFavorites(canonicalFile);
        });
    }

    menu.exec(mAppView->viewport()->mapToGlobal(p));
}

void LXQtFancyMenuWindow::setCurrentCategory(int cat)
{
    QModelIndex idx = mCategoryModel->index(cat, 0);
    mCategoryView->setCurrentIndex(idx);
    mCategoryView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
    mAppModel->setCurrentCategory(cat);

    // If user clicked elsewhere, reset search
    if(cat != LXQtFancyMenuAppMap::AllAppsCategory)
        setSearchQuery(QString());
}

bool LXQtFancyMenuWindow::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == mSearchEdit && e->type() == QEvent::KeyPress)
    {
        QKeyEvent *ev = static_cast<QKeyEvent *>(e);
        if(ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down)
        {
            // Use Up/Down arrows to navigate app view
            QCoreApplication::sendEvent(mAppView, ev);
            return true;
        }
    }
    else if (mAutoSel
             && (watched == mCategoryView->viewport() || watched == mAppView->viewport()))
    {
        if (e->type() == QEvent::MouseMove)
            mAutoSelTimer.start();
        else if (e->type() == QEvent::Leave)
            mAutoSelTimer.stop();
    }

    return QWidget::eventFilter(watched, e);
}

void LXQtFancyMenuWindow::doSearch()
{
    setSearchQuery(mSearchEdit->text());
}

void LXQtFancyMenuWindow::setSearchQuery(const QString &text)
{
    // Do not set text if not changed otherwise cursor
    // gets moved to end of text, it's annoying if user
    // is editing in the middle of text
    if(mSearchEdit->text() != text)
        mSearchEdit->setText(text);

    if(text.isEmpty())
    {
        mAppModel->endSearch();
        return;
    }

    setCurrentCategory(LXQtFancyMenuAppMap::AllAppsCategory);

    auto apps = mAppMap->getMatchingApps(text);
    mAppModel->showSearchResults(apps);
}

void LXQtFancyMenuWindow::hideEvent(QHideEvent *e)
{
    emit aboutToHide();

    if(mFilterClear)
        setSearchQuery(QString()); // Clear search on hide

    // If search is not active, switch to Favorites
    if(mSearchEdit->text().isEmpty())
        setCurrentCategory(LXQtFancyMenuAppMap::FavoritesCategory);

    QWidget::hideEvent(e);
}

void LXQtFancyMenuWindow::showEvent(QShowEvent *e)
{
    // Resize the widget to fit the category view to its contents.
    // NOTE: The layout is fully calculated when the widget is shown;
    // hence resizing the widget here.
    auto model = mCategoryView->model();
    if (model == nullptr)
    {
        QWidget::showEvent(e);
        return;
    }
    QRect r;
    for (int i = 0; i < model->rowCount(); ++i)
    {
        QModelIndex indx = model->index(i,0);
        if (indx.isValid())
        {
            r = r.united(mCategoryView->visualRect(indx));
        }
    }
    QMargins m = mCategoryView->contentsMargins();
    int difW = r.width() + m.left() + m.right() - mCategoryView->width();
    int difH = r.height() + m.top() + m.bottom() - mCategoryView->height();
    if (difW > 0 || difH > 0)
    {
        int newWidth = width();
        int newHeight = height();
        if (difW > 0)
        { // also take stretch factors into account
            newWidth += qreal((CAT_VIEW_STRETCH + APP_VIEW_STRETCH) * difW) / CAT_VIEW_STRETCH + 1
                        + 2 * mCategoryView->lineWidth()
                        + style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        }
        if (difH > 0)
        {
            newHeight += difH
                         + 2 * mCategoryView->lineWidth()
                         + style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        }
        QSize newSize(newWidth, newHeight);

        // take care of small screens or huge fonts
        QRect sr;
        if (QWindow *win = windowHandle())
        {
            if (QScreen *sc = win->screen())
            {
                sr = sc->availableGeometry();
            }
        }
        if (sr.isNull())
        {
            if (QScreen *pScreen = QApplication::primaryScreen())
            {
                sr = pScreen->availableGeometry();
            }
        }
        if (!sr.isNull())
        {
            newSize = newSize.boundedTo(sr.size());
        }

        resize(newSize);
    }

    QWidget::showEvent(e);
}

void LXQtFancyMenuWindow::keyPressEvent(QKeyEvent *e)
{
    // If search edit is not empty, clear it instead of closing popup
    if(!mSearchEdit->text().isEmpty() && e->matches(QKeySequence::Cancel))
    {
        mSearchEdit->clear();
        mSearchTimer.start();
        e->accept();
        return;
    }

    QWidget::keyPressEvent(e);
}

void LXQtFancyMenuWindow::runCommandHelper(const QString &cmd)
{
    if(QProcess::startDetached(cmd, QStringList()))
    {
        hide();
    }
    else
    {
        QMessageBox::warning(this, tr("No Executable"),
                             tr("Cannot find <b>%1</b> executable.").arg(cmd));
    }
}

void LXQtFancyMenuWindow::addToFavorites(const QString &desktopFile)
{
    mFavorites.append(desktopFile);

    mAppModel->reloadAppMap(false);
    mAppMap->addToFavorites(desktopFile);
    mAppModel->reloadAppMap(true);

    emit favoritesChanged();
}

void LXQtFancyMenuWindow::removeFromFavorites(const QString &desktopFile)
{
    mFavorites.removeOne(desktopFile);
    mAppModel->reloadAppMap(false);
    mAppMap->removeFromFavorites(desktopFile);
    mAppModel->reloadAppMap(true);

    emit favoritesChanged();
}

void LXQtFancyMenuWindow::setFilterClear(bool newFilterClear)
{
    mFilterClear = newFilterClear;

    if(mFilterClear && !isVisible())
    {
        // Apply immediately
        setSearchQuery(QString());
    }
}

void LXQtFancyMenuWindow::setButtonPosition(LXQtFancyMenuButtonPosition pos)
{
    mMainLayout->removeItem(mButtonsLayout);
    int idx = 0;
    if(pos == LXQtFancyMenuButtonPosition::Bottom)
        idx = -1;

    mMainLayout->insertLayout(idx, mButtonsLayout);
}

void LXQtFancyMenuWindow::setCategoryPosition(LXQtFancyMenuCategoryPosition pos)
{
    mViewLayout->removeWidget(mCategoryView);
    int idx = 0;
    if(pos == LXQtFancyMenuCategoryPosition::Right)
        idx = -1;

    mViewLayout->insertWidget(idx, mCategoryView, CAT_VIEW_STRETCH);
}

void LXQtFancyMenuWindow::updateButtonIconSize()
{
    int sz = style()->pixelMetric(QStyle::PM_LargeIconSize, nullptr, mSettingsButton);
    const QSize iconSize(sz, sz);
    mSettingsButton->setIconSize(iconSize);
    mPowerButton->setIconSize(iconSize);
}

void LXQtFancyMenuWindow::setSearchEditFocus()
{
    mSearchEdit->setFocus();
}

void LXQtFancyMenuWindow::setCustomFont(const QFont &f)
{
    mAppView->setFont(f);
    mCategoryView->setFont(f);
    mSearchEdit->setFont(f);

    // We should resize the widget because the font may become smaller.
    // The size will be adjusted in showEvent() if needed.
    if (!isVisible())
    {
        resize(minimumSizeHint());
    }
}

QStringList LXQtFancyMenuWindow::favorites() const
{
    return mFavorites;
}

void LXQtFancyMenuWindow::setFavorites(const QStringList &newFavorites)
{
    mFavorites = newFavorites;
    mAppModel->reloadAppMap(false);
    mAppMap->setFavorites(mFavorites);
    mAppModel->reloadAppMap(true);
}

void LXQtFancyMenuWindow::paintEvent(QPaintEvent *)
{
    // enforce the stylesheet background color (if any) on all widget styles
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LXQtFancyMenuWindow::autoSelect()
{
    QModelIndex idx = mCategoryView->indexAt(mCategoryView->viewport()->mapFromGlobal(QCursor::pos()));
    if (idx.isValid())
    {
        if (!SeparatorDelegate::isSeparator(idx) && !mCategoryView->selectionModel()->isSelected(idx))
        {
            activateCategory(idx);
        }
    }
    else
    {
        idx = mAppView->indexAt(mAppView->viewport()->mapFromGlobal(QCursor::pos()));
        if (idx.isValid() && !mAppView->selectionModel()->isSelected(idx))
        {
            mAppView->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::SelectCurrent);
        }
    }
}
