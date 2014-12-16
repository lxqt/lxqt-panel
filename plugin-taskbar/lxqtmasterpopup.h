#ifndef LXQTMASTERPOPUP_H
#define LXQTMASTERPOPUP_H

#include <QFrame>

class QStackedWidget;
class LxQtTaskBar;
class LxQtLooseFocusFrame;
class LxQtTaskGroup;
class LxQtTaskButton;

class LxQtMasterPopup: public QFrame
{
    Q_OBJECT
public:
    static LxQtMasterPopup * instance(LxQtTaskBar * parent);
    LxQtLooseFocusFrame * createFrame(LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    LxQtTaskBar * parentTaskBar();
    void activateGroup(LxQtTaskGroup * group, bool show);
    void activateCloseTimer(bool activate);

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);

private:
    QStackedWidget * mStackedWidget;
    QHash<LxQtTaskGroup *, LxQtLooseFocusFrame *> mGroupHash;
    QTimer * mCloseTimer;
    bool mMouseOnFrame;
    bool mMouseOnGroup;

    void checkTimer();
    LxQtMasterPopup(LxQtTaskBar * parent);

private slots:
    void onGroupDestroyed(QObject *);
};


#endif // LXQTMASTERPOPUP_H
