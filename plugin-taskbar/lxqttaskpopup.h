#ifndef LXQTTASKPOPUP_H
#define LXQTTASKPOPUP_H

#include <QDialog>
#include <QHash>


class QFrame;
class LxQtTaskButton;
class QPropertyAnimation;

class LxQtLooseFocusFrame: public QDialog
{
    Q_OBJECT
public:
    LxQtLooseFocusFrame(QWidget * parent = NULL);
    ~LxQtLooseFocusFrame();

    void moveEyeCandy(const QPoint & newPos);
    void resizeEyeCandy(int w, int h);

    LxQtLooseFocusFrame * instance();
    void setLayout(QLayout * layout, const QHash<WId, LxQtTaskButton *> * buttons);

public slots:
    void startCloseTimer(bool start);

protected:
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void dragEnterEvent(QDragEnterEvent * event);
    void dropEvent(QDropEvent * event);

signals:
    void mouseLeft(bool);

private:

    static LxQtLooseFocusFrame * mSingleInstance;
    const QHash<WId, LxQtTaskButton *> * mButtonHash;
    QPropertyAnimation * mPosAnimation;
    QPropertyAnimation * mSizeAnimation;

private slots:
    void buttonDropped(const QPoint & point, QDropEvent* event);
};



#endif // LXQTTASKPOPUP_H
