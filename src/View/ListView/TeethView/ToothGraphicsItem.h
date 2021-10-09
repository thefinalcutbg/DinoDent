#pragma once
#include <QGraphicsItem>


class ToothGraphicsItem : public QGraphicsItem
{

    QPixmap m_tooth;
    QRect bounds;
    int index;

    bool hasProcedure;

    int procedureMarkerHeight;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
public:

    ToothGraphicsItem(int index);
    ~ToothGraphicsItem();
    
    void showLingual(bool show);
    void setProcedure(bool hasProcedure);
    
    QRectF boundingRect() const override;

    //not working:
    void setToothPixmap(const QPixmap& px);
   

};