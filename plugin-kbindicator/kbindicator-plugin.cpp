#include <QObject>
#include "src/kbdstate.h"
#include "../panel/ilxqtpanelplugin.h"

class LXQtKbIndicatorPlugin: public QObject, public ILXQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILXQtPanelPluginLibrary)
public:
    virtual ~LXQtKbIndicatorPlugin()
    {}

    virtual ILXQtPanelPlugin *instance(const ILXQtPanelPluginStartupInfo &startupInfo) const
    { return new KbdState(startupInfo); }
};

#include "kbindicator-plugin.moc"
