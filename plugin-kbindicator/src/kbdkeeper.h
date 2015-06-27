#ifndef _KBDKEEPER_H_
#define _KBDKEEPER_H_

#include <QHash>
#include <QWidget>
#include "kbdlayout.h"
#include "kbdinfo.h"
#include "settings.h"

//--------------------------------------------------------------------------------------------------

class KbdKeeper: public QObject
{
    Q_OBJECT
public:
    KbdKeeper(const KbdLayout & layout, KeeperType type = KeeperType::Global);
    virtual ~KbdKeeper();
    virtual bool setup();

    const QString & sym() const
    { return m_info.currentSym(); }

    const QString & name() const
    { return m_info.currentName(); }

    const QString & variant() const
    { return m_info.currentVariant(); }

    KeeperType type() const
    { return m_type; }

    void switchToNext();
    virtual void switchToGroup(uint group);
protected slots:
    virtual void keyboardChanged();
    virtual void layoutChanged(uint group);
    virtual void checkState();
signals:
    void changed();
protected:
    const KbdLayout & m_layout;
    KbdInfo           m_info;
    KeeperType        m_type;
};

//--------------------------------------------------------------------------------------------------

class WinKbdKeeper: public KbdKeeper
{
    Q_OBJECT
public:
    WinKbdKeeper(const KbdLayout & layout);
    virtual ~WinKbdKeeper();
    virtual void switchToGroup(uint group);
protected slots:
    virtual void layoutChanged(uint group);
    virtual void checkState();
private:
    QHash<WId, int> m_mapping;
    WId             m_active;
};

//--------------------------------------------------------------------------------------------------

class AppKbdKeeper: public KbdKeeper
{
    Q_OBJECT
public:
    AppKbdKeeper(const KbdLayout & layout);
    virtual ~AppKbdKeeper();
    virtual void switchToGroup(uint group);
protected slots:
    virtual void layoutChanged(uint group);
    virtual void checkState();
private:
    QHash<QString, int> m_mapping;
    QString             m_active;
};

#endif
