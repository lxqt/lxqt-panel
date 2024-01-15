#include "lxqtfancymenushortcututils.h"

#include <QKeyEvent>
#include <QMetaEnum>

bool LXQtFancyMenuShortcutUtils::match(QKeyEvent *event, const QString &sequence)
{
    //TODO: check if still needed or at least document this hack

    static const auto key_meta = QMetaEnum::fromType<Qt::Key>();
    // if our shortcut key is pressed while the menu is open, close the menu
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    QFlags<Qt::KeyboardModifier> mod = keyEvent->modifiers();
    switch (keyEvent->key())
    {
    case Qt::Key_Alt:
        mod &= ~Qt::AltModifier;
        break;
    case Qt::Key_Control:
        mod &= ~Qt::ControlModifier;
        break;
    case Qt::Key_Shift:
        mod &= ~Qt::ShiftModifier;
        break;
    case Qt::Key_Super_L:
    case Qt::Key_Super_R:
        mod &= ~Qt::MetaModifier;
        break;
    }
    const QString press = QKeySequence{static_cast<int>(mod)}.toString() % QString::fromLatin1(key_meta.valueToKey(keyEvent->key())).remove(0, 4);
    return press == sequence;
}
