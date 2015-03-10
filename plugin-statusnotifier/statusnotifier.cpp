#include "statusnotifier.h"

StatusNotifier::StatusNotifier(const ILxQtPanelPluginStartupInfo &startupInfo) :
    QObject(),
    ILxQtPanelPlugin(startupInfo)
{
    m_widget = new StatusNotifierWidget(this);
}

void StatusNotifier::realign()
{
    m_widget->realign();
}
