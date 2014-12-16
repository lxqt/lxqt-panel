#ifndef LXQTTASKPOPUP_H
#define LXQTTASKPOPUP_H

#include <QHash>
#include <QFrame>

class LxQtTaskButton;
class LxQtTaskBar;
class LxQtTaskGroup;
class LxQtMasterPopup;

class LxQtLooseFocusFrame: public QFrame
{
    Q_OBJECT
public:
    LxQtLooseFocusFrame(LxQtMasterPopup * parent, LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    ~LxQtLooseFocusFrame();

protected:
    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent(QDropEvent * event);

private:
    const QHash<WId, LxQtTaskButton *> & mButtonHash;
    LxQtTaskBar * parentTaskBar() ;
    LxQtMasterPopup * parentMasterPopup() ;
    LxQtTaskGroup * mGroup;


private slots:
    void buttonDropped(const QPoint & point, QDropEvent* event);
};


#endif // LXQTTASKPOPUP_H
