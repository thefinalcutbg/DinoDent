#pragma once
#include <QGraphicsItem>

#include  "Presenter/InputEnums.h"

enum class MouseClick {leftClick, rightClick};
enum class Hover{in, out};


class PolygonObserver
{
public:
    virtual void buttonHovered(ButtonPos position, Hover hoverState) = 0;
    virtual void buttonClicked(ButtonPos position, MouseClick click) = 0;
};

class ControlPanelPolygon : public QGraphicsItem
{
 
    PolygonObserver* observer;

    ButtonPos position;
    bool hovered;
    QPolygon poly;

public:
    ControlPanelPolygon(ButtonPos direction, PolygonObserver *observer);

    QRectF boundingRect() const override;

    QPainterPath shape() const override; //required for the hover percision

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
};

class PolygonBorder : public QGraphicsItem
{
    bool is_focused = false;

public:
    void drawFocused(bool focused) {
        is_focused = focused;
        update();
    }
    
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;


};