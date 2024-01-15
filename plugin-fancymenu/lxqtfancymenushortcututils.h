#ifndef LXQTFANCYMENUSHORTCUTUTILS_H
#define LXQTFANCYMENUSHORTCUTUTILS_H

class QString;
class QKeyEvent;

class LXQtFancyMenuShortcutUtils
{
public:
    static bool match(QKeyEvent *event, const QString& sequence);
};

#endif // LXQTFANCYMENUSHORTCUTUTILS_H
