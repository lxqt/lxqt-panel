#include <QSettings>
#include "settings.h"

Settings::Settings()
{}

Settings & Settings::instance()
{
    static Settings _instance;
    return _instance;
}

void Settings::init(QSettings *settings)
{
    m_settings = settings;
    m_oldSettings.reset(new LxQt::SettingsCache(settings));
}

bool Settings::showCapLock() const
{ return m_settings->value("show_caps_lock", true).toBool(); }

bool Settings::showNumLock() const
{ return m_settings->value("show_num_lock", true).toBool(); }

bool Settings::showScrollLock() const
{ return m_settings->value("show_scroll_lock", true).toBool(); }

bool Settings::showLayout() const
{ return m_settings->value("show_layout", true).toBool(); }

void Settings::setShowCapLock(bool show)
{ m_settings->setValue("show_caps_lock", show); }

void Settings::setShowNumLock(bool show)
{ m_settings->setValue("show_num_lock", show); }

void Settings::setShowScrollLock(bool show)
{ m_settings->setValue("show_scroll_lock", show); }

void Settings::setShowLayout(bool show)
{ m_settings->setValue("show_layout", show); }

KeeperType Settings::keeperType() const
{
    QString type = m_settings->value("keeper_type", "global").toString();
    if(type == "global")
        return KeeperType::Global;
    if(type == "window")
        return KeeperType::Window;
    if(type == "application")
        return KeeperType::Application;
    return KeeperType::Application;
}

void Settings::setKeeperType(KeeperType type) const
{
    switch (type) {
    case KeeperType::Global:
        m_settings->setValue("keeper_type", "global");
        break;
    case KeeperType::Window:
        m_settings->setValue("keeper_type", "window");
        break;
    case KeeperType::Application:
        m_settings->setValue("keeper_type", "application");
        break;
    }
}

void Settings::restore()
{ m_oldSettings->loadToSettings(); }
