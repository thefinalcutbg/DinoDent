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

public:
    SelectionBox(int index);
    ~SelectionBox();

protected:
    bool hovered;
    QRectF bounds;
    qreal m_hoverProgress{ 0.0 };
    QVariantAnimation* m_hoverAnimation{ nullptr };

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
    int getIndex();
    void drawFocused(bool focused);
};
