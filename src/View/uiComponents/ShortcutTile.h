#pragma once
#include <QPushButton>

class QVariantAnimation;

class ShortcutTile : public QPushButton
{
    void paintEvent(QPaintEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* e) override;

protected:
    bool hover{ false };
    bool clicked{ false };

    // Hover color animation (0 = normal, 1 = hovered)
    qreal hoverProgress{ 0.0 };
    QVariantAnimation* hoverAnimation{ nullptr };

public:
    ShortcutTile(QWidget* parent = 0);
};
