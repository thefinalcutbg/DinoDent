#include "CPToothPainter.h"


QPixmap CPToothPainter::paintToothStatus()
{
    if (tooth->extraction.exists()) return QPixmap();
    else if (tooth->root.exists()) return QPixmap();
    else if (tooth->implant.exists()) return QPixmap();
    
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    painter.drawPixmap(0, 0, std::move(toothPixmap()));

    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, std::move(surfacePixmap()));

    if (tooth->crown.exists() || tooth->bridge.exists()) {
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
    }

    return toothPx;

}

QPixmap* CPToothPainter::getPixmap(const Tooth& tooth)
{
    int index = tooth.index;

    coords = tooth_type[index];
    this->tooth = &tooth;
    currentIndex = index;

    QPixmap pixmap(150, 150);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = container.getTexture(index, tooth.temporary.exists());

    rotateByQuadrant(painter, 150, 150, currentIndex);

    painter.drawPixmap(QRect(0,0,150,150), paintToothStatus(), coords->CPcrop);

    return new QPixmap(pixmap);
}
