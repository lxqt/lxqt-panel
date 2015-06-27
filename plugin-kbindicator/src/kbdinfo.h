#ifndef _KBDINFO_H_
#define _KBDINFO_H_

#include <QString>
#include <QList>

class KbdInfo
{
public:
    KbdInfo()
    {}

    struct Info
    {
        QString sym;
        QString name;
        QString variant;
    };

public:
    const QString & currentSym() const
    { return m_keyboardInfo[m_current].sym; }

    const QString & currentName() const
    { return m_keyboardInfo[m_current].name; }

    const QString & currentVariant() const
    { return m_keyboardInfo[m_current].variant; }

    int currentGroup() const
    { return m_current; }

    void setCurrentGroup(int group)
    { m_current = group; }

    uint size() const
    { return m_keyboardInfo.size(); }

    const Info & current() const
    { return m_keyboardInfo[m_current]; }

    void clear()
    { m_keyboardInfo.clear(); }

    void append(const Info & info)
    { m_keyboardInfo.append(info); }
private:
    QList<Info> m_keyboardInfo;
    int         m_current = 0;
};

#endif
