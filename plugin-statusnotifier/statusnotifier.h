#ifndef STATUSNOTIFIER_PLUGIN_H
#define STATUSNOTIFIER_PLUGIN_H

#include "../panel/ilxqtpanelplugin.h"
#include "statusnotifierwidget.h"

class StatusNotifier : public QObject, public ILxQtPanelPlugin
{
    Q_OBJECT
public:
    StatusNotifier(const ILxQtPanelPluginStartupInfo &startupInfo);

    bool isSeparate() const { return true; }
    void realign();
    QString themeId() const { return "StatusNotifier"; }
    QWidget *widget() { return m_widget; }

private:
    StatusNotifierWidget *m_widget;
};

class StatusNotifierLibrary : public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
//     Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo)
    {
        return new StatusNotifier(startupInfo);
    }
};

#endif // STATUSNOTIFIER_PLUGIN_H
