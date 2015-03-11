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
    void itemAdded(QString serviceAndPath);
    void itemRemoved(const QString &serviceAndPath);

    void realign();

private:
    LxQt::GridLayout *layout;
    ILxQtPanelPlugin *mPlugin;
    StatusNotifierWatcher *mWatcher;

    QHash<QString, StatusNotifierButton*> mServices;
};

#endif // STATUSNOTIFIERWIDGET_H
