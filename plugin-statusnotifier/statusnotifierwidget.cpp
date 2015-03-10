#include "statusnotifierwidget.h"

StatusNotifierWidget::StatusNotifierWidget(ILxQtPanelPlugin *plugin, QWidget *parent) :
    QWidget(parent),
    m_plugin(plugin)
{
    m_watcher = new StatusNotifierWatcher;

    if (!QDBusConnection::sessionBus().registerService("org.kde.StatusNotifierWatcher"))
    {
        qDebug() << QDBusConnection::sessionBus().lastError().message();
    }
    if (!QDBusConnection::sessionBus().registerObject("/StatusNotifierWatcher", m_watcher, QDBusConnection::ExportScriptableContents))
    {
        qDebug() << QDBusConnection::sessionBus().lastError().message();
    }

    connect(m_watcher, SIGNAL(itemAdded(QString,QString)), this, SLOT(addButton(QString,QString)));
    connect(m_watcher, SIGNAL(itemRemoved(int)), this, SLOT(removeButton(int)));

    layout = new LxQt::GridLayout(this);
    setLayout(layout);
    realign();
}

void StatusNotifierWidget::addButton(QString service, QString objectPath)
{
    StatusNotifierButton *button = new StatusNotifierButton(service, objectPath);

    layout->addWidget(button);
    layout->setAlignment(button, Qt::AlignCenter);
    button->show();
}

void StatusNotifierWidget::removeButton(int index)
{
    layout->itemAt(index)->widget()->deleteLater();
    layout->removeWidget(layout->itemAt(index)->widget());
}

void StatusNotifierWidget::realign()
{
    layout->setEnabled(false);

    ILxQtPanel *panel = m_plugin->panel();
    if (panel->isHorizontal())
    {
        layout->setRowCount(panel->lineCount());
        layout->setColumnCount(0);
    }
    else
    {
        layout->setColumnCount(panel->lineCount());
        layout->setRowCount(0);
    }

    layout->setEnabled(true);
}
