#include "lxqtfancymenucontextmenuutils.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>

#include <QMenu>

#include <QMessageBox>

#include <QFile>
#include <QStandardPaths>
#include <XdgIcon>
#include <XdgDesktopFile>

class LXQtFancyMenuContextMenuStrings
{
public:
    Q_DECLARE_TR_FUNCTIONS(LXQtFancyMenuContextMenuStrings)
};

void LXQtFancyMenuContextMenuUtils::buildContextMenu(QMenu *menu,
                                                     QWidget *window,
                                                     const XdgDesktopFile& desktopFile)
{
    QString file = desktopFile.fileName();

    QAction *a = nullptr;

    if (desktopFile.actions().count() > 0 && desktopFile.type() == XdgDesktopFile::Type::ApplicationType)
    {
        for (int i = 0; i < desktopFile.actions().count(); ++i)
        {
            QString actionString(desktopFile.actions().at(i));
            a = menu->addAction(desktopFile.actionIcon(actionString),
                                desktopFile.actionName(actionString));

            QObject::connect(a, &QAction::triggered, window,
                             [window, desktopFile, actionString]
                             {
                                 desktopFile.actionActivate(actionString, QStringList());
                                 window->hide();
                             });
        }
        menu->addSeparator();
    }

    a = menu->addAction(XdgIcon::fromTheme(QLatin1String("desktop")), LXQtFancyMenuContextMenuStrings::tr("Add to desktop"));
    QObject::connect(a, &QAction::triggered, window,
                     [file, window]
                     {
                         QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
                         QString destinationFile = desktop + QStringLiteral("/") + file.section(QStringLiteral("/"), -1);
                         if (QFile::exists(destinationFile))
                         {
                             QMessageBox::StandardButton btn =
                                 QMessageBox::question(window,
                                                       LXQtFancyMenuContextMenuStrings::tr("Question"),
                                                       LXQtFancyMenuContextMenuStrings::tr("A file with the same name already exists.\n"
                                                                                           "Do you want to overwrite it?"));
                             if (btn == QMessageBox::No)
                                 return;

                             if (!QFile::remove(destinationFile))
                             {
                                 QMessageBox::warning(window,
                                                      LXQtFancyMenuContextMenuStrings::tr("Warning"),
                                                      LXQtFancyMenuContextMenuStrings::tr("The file cannot be overwritten."));
                                 return;
                             }
                         }
                         QFile::copy(file, destinationFile);
                     });

    a = menu->addAction(XdgIcon::fromTheme(QLatin1String("edit-copy")), LXQtFancyMenuContextMenuStrings::tr("Copy"));
    QObject::connect(a, &QAction::triggered, window,
                     [file]
                     {
                         QClipboard* clipboard = QApplication::clipboard();
                         QMimeData* data = new QMimeData();
                         data->setUrls({QUrl::fromLocalFile(file)});
                         clipboard->setMimeData(data);
                     });
}
