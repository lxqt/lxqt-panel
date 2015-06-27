#ifndef _X11KBD_H_
#define _X11KBD_H_

#include <QObject>
#include "../controls.h"

class KbdInfo;

namespace pimpl
{ class X11Kbd; }

class X11Kbd: public QObject
{
    Q_OBJECT
public:
    X11Kbd();
    virtual ~X11Kbd();

    bool init();
    bool isEnabled() const;
    void readKbdInfo(KbdInfo & info) const;
    void lockGroup(uint layId) const;
    void lockModifier(Controls cnt, bool locked);
    bool isModifierLocked(Controls cnt) const;
signals:
    void layoutChanged(uint layId);
    void modifierChanged(Controls cnt, bool locked);
    void checkState();
    void keyboardChanged();
private:
    QScopedPointer<pimpl::X11Kbd> m_priv;
};

#endif
