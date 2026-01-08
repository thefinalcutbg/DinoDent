#pragma once

#include <QGraphicsObject>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

class QVariantAnimation;

class SelectionBox : public QGraphicsObject
{
    Q_OBJECT

    int index;
    bool m_focused = false;
    SelectionBox* left_ptr = nullptr;
    SelectionBox* right_ptr = nullptr;

    bool hovered;
    QRectF bounds;
    qreal m_hoverProgress{ 0.0 };
    QVariantAnimation* m_hoverAnimation{ nullptr };

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
    SelectionBox(int index);
    void setNeighbours(SelectionBox* left, SelectionBox* right) { left_ptr = left; right_ptr = right; }
    int getIndex();
    void drawFocused(bool focused);
    ~SelectionBox();

};
