#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <LXQt/Settings>
class QSettings;

enum class KeeperType
{
    Global,
    Window,
    Application
};

class Settings
{
public:
    Settings();
    static Settings & instance();

    void init(QSettings *settings);

public:
    bool showCapLock() const;
    bool showNumLock() const;
    bool showScrollLock() const;
    bool showLayout() const;
    KeeperType keeperType() const;
    void restore();
public:
    void setShowCapLock(bool show);
    void setShowNumLock(bool show);
    void setShowScrollLock(bool show);
    void setShowLayout(bool show);
    void setKeeperType(KeeperType type) const;
private:
    QSettings                           *m_settings = 0;
    QScopedPointer<LxQt::SettingsCache>  m_oldSettings;
};

#endif
