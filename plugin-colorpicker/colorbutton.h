#pragma once

#include <QToolButton>


class QColor;
class QPaintEvent;


class ColorButton : public QToolButton
{
    public:

        void paintEvent(QPaintEvent* event);
        void setColor(QColor color);

    private:

        QColor color;
};
