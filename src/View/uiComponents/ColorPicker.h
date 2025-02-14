#pragma once

#include <QPushButton>

class ColorPicker : public QPushButton
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
