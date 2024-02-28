#include "CPTooth.h"

CPTooth::CPTooth() //: texture(nullptr)
{}

CPTooth::~CPTooth()
{
    //delete texture;
}

QRectF CPTooth::boundingRect() const
{
    return QRectF(0, 0, 100, 100);
}

void CPTooth::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{

    painter->drawPixmap(0, 0, 100, 100, texture);
    
}

void CPTooth::setPixmap(const QPixmap& texture)
{
    
    this->texture = texture; //texture is sent from the selected tooth in the main scene

    update();
}
