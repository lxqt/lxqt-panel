#ifndef _LXQTKBDINFO_H_
#define _LXQTKBDINFO_H_

#include <QString>

class LxQtKbdInfo
{
public:
    LxQtKbdInfo()
    {}

    struct Info
    {
        QString sym;
        QString name;
    };

public:
    bool readKeyboardInfo();

    const QString & currentSym() const
    { return m_keyboardInfo[m_current].sym; }

    int currentGroup() const
    { return m_current; }

    void setCurrentGroup(int group)
    { m_current = group; }

    const QString & currentName() const
    { return m_keyboardInfo[m_current].name; }

    int size() const
    { return m_keyboardInfo.size(); }

    const Info & current() const
    { return m_keyboardInfo[m_current]; }

private:
    void readRules();
    void readCurrent();
private:
    QList<Info> m_keyboardInfo;
    int         m_current = 0;
};

#endif
