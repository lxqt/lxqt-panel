#ifndef LXQTTASKGROUP_H
#define LXQTTASKGROUP_H

#include <QDialog>
#include "lxqttaskbutton.h"
#include <KF5/KWindowSystem/kwindowsystem.h>

class QVBoxLayout;
class ILxQtPanelPlugin;

class LxQtLooseFocusFrame;

class LxQtTaskGroup: public LxQtTaskButton
{
    Q_OBJECT
public:
    LxQtTaskGroup(const QString & groupName, QIcon icon ,ILxQtPanelPlugin * plugin, LxQtTaskBar * parent);

    void removeButton(WId window);
    int buttonsCount() const;
    int visibleButtonsCount(LxQtTaskButton ** first = NULL) const;


    LxQtTaskButton * createButton(WId id);
    LxQtTaskButton * checkedButton() const;
    bool checkNextPrevChild(bool next, bool modulo);

    QString groupName() const {return mGroupName;}

    void refreshIconsGeometry();
    void showOnAllDesktopSettingChanged();
    void setAutoRotation(bool value, ILxQtPanel::Position position);
    void setToolButtonsStyle(Qt::ToolButtonStyle style);


protected:
    void arbitraryMimeData(QMimeData * mime);

    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);


private slots:
    void onClicked(bool checked);
    void onChildButtonClicked();
    void onActiveWindowChanged(WId window);
    void onWindowRemoved(WId window);
    void onDesktopChanged(int number);
    void windowChanged(WId window, NET::Properties prop, NET::Properties2 prop2);

    void frameFocusLost();
    void mouseFrameChanged(bool left);
    void timeoutClose(void);


signals:
    void groupBecomeEmpty(QString name);

private:


    QString mGroupName;
    QDialog * mFrame;
    LxQtTaskButtonHash mButtonHash;
    QVBoxLayout * mLayout;
    ILxQtPanelPlugin * mPlugin;
    QTimer * mTimer;

    void raisePopup(bool raise);
    void recalculateFrameHeight();
    void refreshVisibility();
    void regroup(void);
    void timerEnable(bool enable);


};

class LxQtLooseFocusFrame: public QDialog
{
    Q_OBJECT
public:
    LxQtLooseFocusFrame(QWidget * parent = NULL);
    ~LxQtLooseFocusFrame();

protected:
    void focusOutEvent(QFocusEvent * event);
    void showEvent(QShowEvent * event);
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);

signals:
    void focusLost(void);
    void mouseLeft(bool);
};

#endif // LXQTTASKGROUP_H
