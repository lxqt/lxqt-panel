#include "statusnotifierbutton.h"

StatusNotifierButton::StatusNotifierButton(QString service, QString objectPath, QWidget *parent)
    : QToolButton(parent),
    mStatus(Passive)
{
    interface = new org::kde::StatusNotifierItem(service, objectPath, QDBusConnection::sessionBus());

    newIcon();
    newToolTip();

    connect(interface, SIGNAL(NewIcon()), this, SLOT(newIcon()));
    connect(interface, SIGNAL(NewToolTip()), this, SLOT(newToolTip()));
    connect(interface, SIGNAL(NewStatus(QString)), this, SLOT(newStatus(QString)));
}

void StatusNotifierButton::contextMenuEvent(QContextMenuEvent* event)
{
    interface->ContextMenu(QCursor::pos().x(), QCursor::pos().y());
    // QWidget::contextMenuEvent(event);
}

void StatusNotifierButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        interface->Activate(QCursor::pos().x(), QCursor::pos().y());
    else if (event->button() == Qt::MidButton)
        interface->SecondaryActivate(QCursor::pos().x(), QCursor::pos().y());
    QToolButton::mouseReleaseEvent(event);
}

void StatusNotifierButton::wheelEvent(QWheelEvent *event)
{
    interface->Scroll(event->delta(), "vertical");
}

void StatusNotifierButton::newIcon()
{
    QString iconName;
    if (mStatus == Active)
        iconName = interface->overlayIconName();
    else if (mStatus == NeedsAttention)
        iconName = interface->attentionIconName();
    else // mStatus == Passive
        iconName = interface->iconName();

    QIcon icon;
    if (!iconName.isEmpty())
    {
        if (icon.hasThemeIcon(iconName))
            icon = QIcon::fromTheme(iconName);
        else
        {
            QDir themeDir(interface->iconThemePath());
            if (themeDir.exists(iconName + ".png"))
                icon.addFile(themeDir.filePath(iconName + ".png"));

            if (themeDir.cd("hicolor"))
            {
                QStringList sizes = themeDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                foreach (QString dir, sizes)
                {
                    QStringList dirs = QDir(themeDir.filePath(dir)).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
                    icon.addFile(QDir(themeDir.path() + "/" + dir + "/" + dirs.at(0)).filePath(iconName + ".png"));
                }
            }

            if (icon.isNull())
                icon = QIcon::fromTheme("application-x-executable");
        }
    }
    else
    {
        IconPixmapList iconPixmaps;
        if (mStatus == Active)
            iconPixmaps = interface->overlayIconPixmap();
        else if (mStatus == NeedsAttention)
            iconPixmaps = interface->attentionIconPixmap();
        else // mStatus == Passive
            iconPixmaps = interface->iconPixmap();

        if (iconPixmaps.isEmpty() || iconPixmaps.first().bytes.isEmpty())
            icon = QIcon::fromTheme("application-x-executable");
        else
        {
            IconPixmap iconPixmap = iconPixmaps.first();
            QImage image((uchar*) iconPixmap.bytes.data(), iconPixmap.width, iconPixmap.height, QImage::Format_ARGB32);
            const uchar *end = image.constBits() + image.byteCount();
            uchar *dest = reinterpret_cast<uchar*>(iconPixmap.bytes.data());
            for (const uchar *src = image.constBits(); src < end; src += 4, dest += 4)
                qToUnaligned(qToBigEndian<quint32>(qFromUnaligned<quint32>(src)), dest);

            image = QImage((uchar*) iconPixmap.bytes.data(), iconPixmap.width, iconPixmap.height, QImage::Format_ARGB32);
            QPixmap pixmap = QPixmap::fromImage(image);
            icon = QIcon(pixmap);
        }
    }

    setIcon(icon);
}

void StatusNotifierButton::newToolTip()
{
    QString toolTipTitle = interface->toolTip().title;
    setToolTip(toolTipTitle.isEmpty() ? interface->title() : toolTipTitle);
}

void StatusNotifierButton::newStatus(QString status)
{
    Status newStatus;
    if (status == QStringLiteral("Passive"))
        newStatus = Passive;
    else if (status == QStringLiteral("Active"))
        newStatus = Active;
    else
        newStatus = NeedsAttention;

    if (mStatus == newStatus)
        return;

    mStatus = newStatus;
    newIcon();
}
