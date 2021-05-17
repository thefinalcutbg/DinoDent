#include "CPToothPainter.h"


QPixmap CPToothPainter::returnPaintedTooth(const PaintHint& tooth)
{
    if (tooth.tooth != ToothHint::normal) return QPixmap();
    
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));

    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, drawSurfaces(tooth));
    painter.setOpacity(1);

 //   painter.setOpacity(1);

    if (tooth.prostho != ProsthoHint::none)
    {
        if (tooth.prostho == ProsthoHint::crown || tooth.prostho == ProsthoHint::bridge)
        {
            painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
        }
        else
        {
            painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
            painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop, Qt::green, 0.3));
        }
            
    }

    return toothPx;

}

QPixmap* CPToothPainter::paintTooth(const PaintHint& tooth)
{
    int index = tooth.idx;

    tooth.temp ?
        coords = temp_tooth_type[tooth.idx]
        :
        coords = tooth_type[tooth.idx];

    QPixmap pixmap(150, 150);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = container.getTexture(index, tooth.temp);

    rotateByQuadrant(painter, 150, 150, index);

    painter.drawPixmap(QRect(0,0,150,150), returnPaintedTooth(tooth), coords->CPcrop);

    return new QPixmap(pixmap);
}
