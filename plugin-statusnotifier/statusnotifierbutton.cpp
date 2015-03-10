#include "statusnotifierbutton.h"

StatusNotifierButton::StatusNotifierButton(QString service, QString objectPath) : menu(0)
{
    interface = new org::kde::StatusNotifierItem(service, objectPath, QDBusConnection::sessionBus());

    newIcon();
    newToolTip();

    QString menuPath = interface->menu().path();
    if (!menuPath.isEmpty()) {
        DBusMenuImporter *menuImporter = new DBusMenuImporter(service, menuPath);
        menu = menuImporter->menu();
    }

    connect(interface, SIGNAL(NewIcon()), this, SLOT(newIcon()));
    connect(interface, SIGNAL(NewToolTip()), this, SLOT(newToolTip()));

    connect(this, SIGNAL(clicked()), this, SLOT(showMenu()));

}

void StatusNotifierButton::showMenu()
{
    if (menu != NULL)
    {
        menu->exec(this->mapToGlobal(QPoint(0, 0)));
    }
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton) {
        setDown(false);
        interface->Activate(QCursor::pos().x(), QCursor::pos().y());
    }
    QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent *event)
{
    interface->Scroll(event->delta(), "vertical");
}

void StatusNotifierButton::newIcon()
{
    QString attentionIconName = interface->attentionIconName();
    QString iconName = attentionIconName.isEmpty() ?
                interface->iconName() : attentionIconName;

    if (!iconName.isEmpty())
    {
        QString iconThemePath = interface->iconThemePath();

        QIcon icon;
        if (icon.hasThemeIcon(iconName))
        {
            icon = QIcon::fromTheme(iconName);
            setIcon(icon);
        }
        else
        {
            QDir themeDir(iconThemePath);
            if (themeDir.exists(iconName + ".png"))
            {
                icon.addFile(themeDir.filePath(iconName + ".png"));
            }

            if (themeDir.exists("hicolor"))
            {
                themeDir.cd("hicolor");

                QStringList sizes = themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                foreach (QString dir, sizes)
                {
                    QStringList dirs = QDir(themeDir.filePath(dir)).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                    icon.addFile(QDir(themeDir.path() + "/" + dir + "/" + dirs.at(0)).filePath(iconName + ".png"));
                }
            }

            if (icon.isNull()) {
                icon = QIcon::fromTheme("image-missing");
                setIcon(icon);
            }
            else
            {
                setIcon(icon);
            }
        }
    }
    else
    {
        QList<IconPixmap> attentionIconPixmaps = interface->attentionIconPixmap();
        QList<IconPixmap> iconPixmaps = attentionIconPixmaps;
        if (attentionIconPixmaps.isEmpty())
        {
            iconPixmaps = interface->iconPixmap();
        }
        else if (attentionIconPixmaps.first().bytes.isEmpty())
        {
            iconPixmaps = interface->iconPixmap();
        }

        if (iconPixmaps.isEmpty())
        {
            setIcon(QIcon::fromTheme("image-missing"));
            return;
        }
        else if (iconPixmaps.first().bytes.isEmpty())
        {
            setIcon(QIcon::fromTheme("image-missing"));
            return;
        }

        IconPixmap iconPixmap = iconPixmaps.first();
        QImage image((uchar*) iconPixmap.bytes.data(), iconPixmap.width, iconPixmap.height, QImage::Format_ARGB32);
        const uchar *end = image.constBits() + image.byteCount();
        uchar *dest = reinterpret_cast<uchar*>(iconPixmap.bytes.data());
        for (const uchar *src = image.constBits(); src < end; src += 4, dest += 4)
            qToUnaligned(qToBigEndian<quint32>(qFromUnaligned<quint32>(src)), dest);

        image = QImage((uchar*) iconPixmap.bytes.data(), iconPixmap.width, iconPixmap.height, QImage::Format_ARGB32);
        QPixmap pixmap = QPixmap::fromImage(image);
        QIcon icon(pixmap);
        setIcon(icon);
    }
}

void StatusNotifierButton::newToolTip()
{
    QString toolTipTitle = interface->toolTip().title;

    if (toolTipTitle.isEmpty())
    {
        setToolTip(interface->title());
    }
    else
    {
        setToolTip(toolTipTitle);
    }
}
