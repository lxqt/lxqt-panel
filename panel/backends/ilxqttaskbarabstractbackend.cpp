#include "ilxqttaskbarabstractbackend.h"


ILXQtTaskbarAbstractBackend::ILXQtTaskbarAbstractBackend(QObject *parent)
    : QObject(parent)
{

}

void ILXQtTaskbarAbstractBackend::moveApplicationToPrevNextDesktop(WId windowId, bool next)
{
    int count = getWorkspacesCount();
    if (count <= 1)
        return;

    int targetWorkspace = getWindowWorkspace(windowId) + (next ? 1 : -1);

    // Wrap around
    if (targetWorkspace > count)
        targetWorkspace = 1; //Ids are 1-based
    else if (targetWorkspace < 1)
        targetWorkspace = count;

    setWindowOnWorkspace(windowId, targetWorkspace);
}
