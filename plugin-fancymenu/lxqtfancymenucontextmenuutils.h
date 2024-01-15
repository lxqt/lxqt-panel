#ifndef LXQTFANCYMENUCONTEXTMENUUTILS_H
#define LXQTFANCYMENUCONTEXTMENUUTILS_H

class QWidget;
class QMenu;
class XdgDesktopFile;

class LXQtFancyMenuContextMenuUtils
{
public:
    static void buildContextMenu(QMenu *menu,
                                 QWidget *window,
                                 const XdgDesktopFile &desktopFile);
};

#endif // LXQTFANCYMENUCONTEXTMENUUTILS_H
