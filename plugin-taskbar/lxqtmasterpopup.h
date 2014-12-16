#ifndef LXQTMASTERPOPUP_H
#define LXQTMASTERPOPUP_H

#include <QFrame>

class QStackedWidget;
class LxQtTaskBar;
class LxQtTaskGroup;
class LxQtTaskButton;
class LxQtGroupPopup;

class LxQtMasterPopup: public QFrame
{
    Q_OBJECT
public:
    static LxQtMasterPopup * instance(LxQtTaskBar * parent);
    LxQtGroupPopup * createFrame(LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    LxQtTaskBar * parentTaskBar();
    void activateGroup(LxQtTaskGroup * group, bool show);
    void activateCloseTimer(LxQtTaskGroup * group, bool activate);

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);

private:
    QStackedWidget * mStackedWidget;
    QHash<LxQtTaskGroup *, LxQtGroupPopup *> mGroupHash;
    QTimer * mCloseTimer;
    bool mMouseOnFrame;
    bool mMouseOnGroup;

    void checkTimer();
    LxQtMasterPopup(LxQtTaskBar * parent);

private slots:
    void onGroupDestroyed(QObject *);
};


#endif // LXQTMASTERPOPUP_H
