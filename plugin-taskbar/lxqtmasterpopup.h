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

    typedef enum {MOUSE_ON_FRAME = 1, MOUSE_ON_GROUP = 2, DRAGGING_ACTIVE = 4} close_flags_enum;
    Q_DECLARE_FLAGS(noCloseFlags_t,close_flags_enum)

public slots:
    void dragging(bool executing);

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void showEvent(QShowEvent * event) ;
    void closeEvent(QCloseEvent * event);

private:
    QStackedWidget * mStackedWidget;
    QHash<LxQtTaskGroup *, LxQtGroupPopup *> mGroupHash;
    QTimer * mCloseTimer;
    noCloseFlags_t mFlags;

    void checkTimer();
    LxQtMasterPopup(LxQtTaskBar * parent);
    QPropertyAnimation * mPosAnimation;
    QPropertyAnimation * mSizeAnimation;
    void setFlag(noCloseFlags_t flag , bool set = true);

private slots:
    void onGroupDestroyed(QObject *);
};


Q_DECLARE_OPERATORS_FOR_FLAGS(LxQtMasterPopup::noCloseFlags_t)

#endif // LXQTMASTERPOPUP_H
