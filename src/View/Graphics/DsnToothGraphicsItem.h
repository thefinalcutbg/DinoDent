#pragma once
#include <QGraphicsItem>


class DsnToothGraphicsItem : public QGraphicsItem
{
    QRectF bounds;
    int index;

    int pxWidth;
    int pxHeight;

    QPixmap m_tooth;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
public:

    DsnToothGraphicsItem(int index);
    ~DsnToothGraphicsItem();
    
    void showLingual(bool show);
    void setWidth(int width);
    QRectF boundingRect() const override;

    //not working:
    void setToothPixmap(const QPixmap& px);
   

};