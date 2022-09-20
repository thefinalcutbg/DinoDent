#pragma once
#include <QGraphicsItem>
#include <QPainter>

class CPTooth : public QGraphicsItem
{
    QPixmap texture;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

public:
    CPTooth();
    ~CPTooth();

    QRectF boundingRect() const override;

    void setPixmap(const QPixmap& texture);
};