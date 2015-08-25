#ifndef _KDBSTATE_H_
#define _KDBSTATE_H_

#include "../panel/ilxqtpanelplugin.h"
#include "settings.h"
#include "content.h"
#include "kbdwatcher.h"

class QLabel;

class KbdState : public QObject, public ILXQtPanelPlugin
{
    Q_OBJECT
public:
    KbdState(const ILXQtPanelPluginStartupInfo &startupInfo);
    virtual ~KbdState();

    virtual QString themeId() const
    { return "KbIndicator"; }

    virtual ILXQtPanelPlugin::Flags flags() const
    { return PreferRightAlignment | HaveConfigDialog; }

    virtual bool isSeparate() const
    { return false; }

    virtual QWidget *widget()
    { return &m_content; }

    QDialog *configureDialog();
    virtual void realign();

    const Settings & prefs() const
    { return m_settings; }

    Settings & prefs()
    { return m_settings; }

protected slots:
    virtual void settingsChanged();

private:
    Settings    m_settings;
    KbdWatcher  m_watcher;
    Content     m_content;
};


#endif
