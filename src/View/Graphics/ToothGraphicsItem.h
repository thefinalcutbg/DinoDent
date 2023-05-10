#pragma once
#include <QGraphicsItem>


class ToothGraphicsItem : public QGraphicsItem
{
    QRectF bounds;
    int index;


    int pxWidth;
    int pxHeight;

    bool hasProcedure;
    bool hasNote;

    int procedureMarkerHeight;

    QPixmap m_tooth;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    
public:

    ToothGraphicsItem(int index);
    ~ToothGraphicsItem();
    
    void showLingual(bool show);
    void setProcedure(bool hasProcedure);
    void setNote(bool hasNote);
    void setWidth(int width);
    QRectF boundingRect() const override;

    //not working:
    void setToothPixmap(const QPixmap& px);
    void drawEmpty();

};