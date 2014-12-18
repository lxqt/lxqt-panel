#ifndef LXQTMASTERPOPUP_H
#define LXQTMASTERPOPUP_H

#include <QFrame>

class QStackedWidget;
class LxQtTaskBar;
class LxQtTaskGroup;
class LxQtTaskButton;
class LxQtGroupPopup;
class ILxQtPanelPlugin;
class QPropertyAnimation;

class LxQtMasterPopup: public QFrame
{
    Q_OBJECT
public:
    static LxQtMasterPopup * instance(LxQtTaskBar * parent);
    LxQtGroupPopup * createFrame(LxQtTaskGroup * group, const QHash<WId, LxQtTaskButton*> & buttons);
    LxQtTaskBar * parentTaskBar();
    void activateGroup(LxQtTaskGroup * group, bool show);
    void activateCloseTimer(LxQtTaskGroup * group, bool activate);
    void moveAnimated(const QPoint & newpos);
    void resizeAnimated(const QSize & newsize);

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void showEvent(QShowEvent * event) ;
    void closeEvent(QCloseEvent * event);

private:
    QStackedWidget * mStackedWidget;
    QHash<LxQtTaskGroup *, LxQtGroupPopup *> mGroupHash;
    QTimer * mCloseTimer;
    bool mMouseOnFrame;
    bool mMouseOnGroup;

    void checkTimer();
    LxQtMasterPopup(LxQtTaskBar * parent);
    QPropertyAnimation * mPosAnimation;
    QPropertyAnimation * mSizeAnimation;

private slots:
    void onGroupDestroyed(QObject *);
};


#endif // LXQTMASTERPOPUP_H
