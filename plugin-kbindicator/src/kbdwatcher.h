#ifndef _KBDWATCHER_H_
#define _KBDWATCHER_H_

#include "kbdlayout.h"
#include "controls.h"
#include "kbdkeeper.h"

class KbdKeeper;

class KbdWatcher: public QObject
{
    Q_OBJECT
public:
    KbdWatcher();

    void setup();
    const KbdLayout & kbdLayout() const
    { return m_layout; }

    bool isLayoutEnabled() const
    { return m_layout.isEnabled(); }
public slots:
    void controlClicked(Controls cnt);
signals:
    void layoutChanged(const QString & sym, const QString & name, const QString & variant);
    void modifierStateChanged(Controls mod, bool active);

private:
    void createKeeper(KeeperType type);
private slots:
    void keeperChanged();

private:
    KbdLayout                 m_layout;
    QScopedPointer<KbdKeeper> m_keeper;
};

#endif
