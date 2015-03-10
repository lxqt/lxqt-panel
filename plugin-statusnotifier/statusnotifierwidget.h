#ifndef STATUSNOTIFIERWIDGET_H
#define STATUSNOTIFIERWIDGET_H

#include <QDir>

#include <LXQt/GridLayout>

#include "statusnotifierbutton.h"
#include "statusnotifierwatcher.h"

class StatusNotifierWidget : public QWidget
{
    Q_OBJECT

public:
    StatusNotifierWidget(ILxQtPanelPlugin *plugin, QWidget *parent = 0);

signals:

public slots:
    void addButton(QString service, QString objectPath);
    void removeButton(int index);

    void realign();

private:
    LxQt::GridLayout *layout;
    ILxQtPanelPlugin *m_plugin;
    StatusNotifierWatcher *m_watcher;
};

#endif // STATUSNOTIFIERWIDGET_H
