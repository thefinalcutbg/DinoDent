#pragma once

#include <QToolButton>

class ColorPicker : public QToolButton
{
    Q_OBJECT

    static QIcon createIcon(const QColor& color, const QRect& rect);

public:
    ColorPicker(QWidget* widget = 0);

public:
    QColor color() const;
    void setColor(const QColor& color);
private:
    QColor selected_color;
};
