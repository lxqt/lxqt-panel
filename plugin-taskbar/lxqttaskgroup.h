#ifndef LXQTTASKGROUP_H
#define LXQTTASKGROUP_H

#include <QFrame>
#include "lxqttaskbutton.h"


class QVBoxLayout;
class ILxQtPanelPlugin;

class LxQtTaskGroup: public LxQtTaskButton
{
    Q_OBJECT
public:
    LxQtTaskGroup(const QString & groupName, QIcon icon ,ILxQtPanelPlugin * plugin, QWidget * parent);

    bool addButton(LxQtTaskButton * button);
    void removeButton(LxQtTaskButton * button);
    int buttonsCount() const;
    QString groupName() const {return mGroupName;}

protected:
    void arbitraryMimeData(QMimeData * mime);


private slots:
    void onClicked(bool checked);

private:
    void regroup(void);

    QString mGroupName;
    QFrame * mFrame;
    LxQtTaskButtonHash mButtonHash;
    QVBoxLayout * mLayout;
    ILxQtPanelPlugin * mPlugin;

};

#endif // LXQTTASKGROUP_H
