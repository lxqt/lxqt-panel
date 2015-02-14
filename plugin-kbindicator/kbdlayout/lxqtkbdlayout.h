#ifndef LXQTKBDLAYOUT_H
#define LXQTKBDLAYOUT_H

#ifdef ENABLE_KBDLAYOUT

#include "kbdlayout/lxqtkbdinfo.h"
#include <QLabel>
class LxQtKbdLayoutPrivate;
class QSettings;

class LxQtKbdLayout
{
public:
    LxQtKbdLayout(QSettings *settings);
    ~LxQtKbdLayout();

    bool enabled() const
    { return m_enabled; }

    QWidget * widget() const
    { return m_widget; }

    void switchNext();

friend class LxQtKbdLayoutPrivate;
private:
    void changed(const QString & sym, const QString & title);
private:
    QLabel               *m_widget;
    LxQtKbdLayoutPrivate *m_layout;
    bool                  m_enabled;
};

#else
class LxQtKbdLayout
{
public:
    LxQtKbdLayout(QSettings */*settings*/)
    {}

    bool enabled() const
    { return false; }

    QWidget * widget() const
    { return 0; }

    void switchNext()
    {}
};
#endif

#endif
