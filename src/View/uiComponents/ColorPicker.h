#pragma once

#include <QPushButton>

class ColorPicker : public QPushButton
{
    Q_OBJECT

    static QIcon createIcon(const QColor& color, const QRect& rect);
    QMenu* contextMenu = nullptr;

public:
    ColorPicker(QWidget* widget = 0);

public:
    QColor color() const;
    void setColor(const QColor& color);
    void addCustomColors(const std::vector<QColor>& colors);
private:
    QColor selected_color;
};
