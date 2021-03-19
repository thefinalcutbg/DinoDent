#pragma once
#include <QGraphicsItem>
#include <QPainter>

class ToothGraphicsItem : public QGraphicsItem
{
    QPixmap* toothGraphic;
    QRectF bounds;
    int index;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

public:

    ToothGraphicsItem(int index);
    ~ToothGraphicsItem();

    void setToothGraphic(QPixmap* toothGraphic);
    QRectF boundingRect() const override;

};