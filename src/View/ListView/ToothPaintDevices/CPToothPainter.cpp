#include "CPToothPainter.h"


QPixmap CPToothPainter::returnPaintedTooth(const PaintHint& tooth)
{
    if (tooth.tooth != ToothHint::normal) return QPixmap();
    
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));


    QPixmap surface(coords->toothCrop.width(), coords->toothCrop.height());
    surface.fill(Qt::transparent);
    QPainter surfPainter(&surface);

    for (int i = 0; i < tooth.surfaces.size(); i++) //drawing the surfaces;
    {
        switch (tooth.surfaces[i])
        {
        case SurfaceHint::normal:
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i]));
            break;
        case SurfaceHint::blue:
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::blue, 1));
            break;
        case SurfaceHint::red:
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::red, 1));
            break;
        case SurfaceHint::green:
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::green, 1));
            break;
        default:
            break;
        }
    }

    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, surface);
 //   painter.setOpacity(1);

    if (tooth.prostho != ProsthoHint::none) {
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
    }

    return toothPx;

}

QPixmap* CPToothPainter::paintTooth(const PaintHint& tooth)
{
    int index = tooth.idx;

    coords = tooth_type[index];

    QPixmap pixmap(150, 150);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = container.getTexture(index, tooth.temp);

    rotateByQuadrant(painter, 150, 150, index);

    painter.drawPixmap(QRect(0,0,150,150), returnPaintedTooth(tooth), coords->CPcrop);

    return new QPixmap(pixmap);
}
