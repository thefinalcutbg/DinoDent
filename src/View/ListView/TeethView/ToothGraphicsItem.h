#pragma once
#include <QGraphicsItem>


class ToothGraphicsItem : public QGraphicsItem
{
    QPixmap* toothGraphic;
    QPixmap m_tooth;
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
    //not working?
    void setToothPixmap(const QPixmap& px);
    QRectF boundingRect() const override;

};