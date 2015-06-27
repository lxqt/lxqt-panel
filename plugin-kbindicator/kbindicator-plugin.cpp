#include <QObject>
#include "src/kbdstate.h"
#include "../panel/ilxqtpanelplugin.h"

class LxQtKbIndicatorPlugin: public QObject, public ILxQtPanelPluginLibrary
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "lxde-qt.org/Panel/PluginInterface/3.0")
    Q_INTERFACES(ILxQtPanelPluginLibrary)
public:
    virtual ~LxQtKbIndicatorPlugin()
    {}

    virtual ILxQtPanelPlugin *instance(const ILxQtPanelPluginStartupInfo &startupInfo) const
    { return new KbdState(startupInfo); }
};

#include "kbindicator-plugin.moc"
