#include "CPTooth.h"
#include <QDebug>

CPTooth::CPTooth() : texture(nullptr)
{}

CPTooth::~CPTooth()
{
    delete texture;
}

QRectF CPTooth::boundingRect() const
{
    return QRectF(0, 0, 100, 100);
}

void CPTooth::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (texture != NULL)
    {
        painter->drawPixmap(0, 0, 100, 100, *texture);
    }
    
}

void CPTooth::setPixmap(QPixmap* texture)
{
    
    if (texture != NULL) {
        delete this->texture;
    }
    
    this->texture = texture; //texture is sent from the selected tooth in the main scene

    update();
}