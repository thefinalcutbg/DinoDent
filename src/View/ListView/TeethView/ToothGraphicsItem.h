#pragma once
#include <QGraphicsItem>


class ToothGraphicsItem : public QGraphicsItem
{
    QPixmap* toothGraphic;
    QRectF bounds;
    int index;

    bool hasProcedure;

    int procedureMarkerHeight;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

public:

    ToothGraphicsItem(int index);
    ~ToothGraphicsItem();


    void setProcedure(bool hasProcedure);
    void setToothGraphic(QPixmap* toothGraphic);
    QRectF boundingRect() const override;

};