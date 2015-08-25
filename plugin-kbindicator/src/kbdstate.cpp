#include <QTimer>
#include <QEvent>

#include "kbdstate.h"
#include "kbdkeeper.h"
#include "kbdstateconfig.h"
#include <LXQt/lxqtsettings.h>

KbdState::KbdState(const ILXQtPanelPluginStartupInfo &startupInfo):
    QObject(),
    ILXQtPanelPlugin(startupInfo),
    m_content(m_watcher.isLayoutEnabled())
{
    Settings::instance().init(settings());

    connect(&m_content, &Content::controlClicked, &m_watcher, &KbdWatcher::controlClicked);
    connect(&m_watcher, &KbdWatcher::layoutChanged, &m_content, &Content::layoutChanged);
    connect(&m_watcher, &KbdWatcher::modifierStateChanged, &m_content, &Content::modifierStateChanged);

    settingsChanged();
}

KbdState::~KbdState()
{}

void KbdState::settingsChanged()
{
    m_content.setup();
    m_watcher.setup();
}

QDialog *KbdState::configureDialog()
{
    return new KbdStateConfig(&m_content);
}

void KbdState::realign()
{
    if (panel()->isHorizontal()){
        m_content.setMinimumSize(0, panel()->iconSize());
        m_content.showHorizontal();
    } else {
        m_content.setMinimumSize(panel()->iconSize(), 0);
        m_content.showVertical();
    }
}
