#ifndef _CONTENT_H_
#define _CONTENT_H_

#include <QWidget>
#include "controls.h"

class QLabel;

class Content : public QWidget
{
    Q_OBJECT
public:
    Content(bool layoutEnabled);
    ~Content();

public:
    void setEnabled(Controls cnt, bool enabled);
    QWidget* widget(Controls cnt) const;
    bool setup();

    virtual bool eventFilter(QObject *object, QEvent *event);

    void showHorizontal();
    void showVertical();
public slots:
    void layoutChanged(const QString & sym, const QString & name, const QString & variant);
    void modifierStateChanged(Controls mod, bool active);
signals:
    void controlClicked(Controls cnt);
private:
    bool        m_layoutEnabled;
    QLabel     *m_capsLock;
    QLabel     *m_numLock;
    QLabel     *m_scrollLock;
    QLabel     *m_layout;
};

#endif
