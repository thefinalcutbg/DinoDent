#include "ToothPainter.h"



ToothPainter::ToothPainter() :
    currentTexture(NULL), 
    currentIndex(0),
    coords(NULL), 
    tooth(NULL),
    molarCoordinates(SpriteRectType::molar),
    premolarCoordinates(SpriteRectType::premolar),
    frontalCoordinates(SpriteRectType::frontal)
{
    

        for (int i = 0; i < 32; i++) //mapping indexes to tooth types
        {
            if (i == 3 || i == 4 || i == 11 || i == 12)
                tooth_type[i] = &premolarCoordinates;
            else if (i == 19 || i == 20 || i == 27 || i == 28)
                tooth_type[i] = &premolarCoordinates;
            else if ((i > 2 && i < 13) || (i > 18 && i < 29))
                tooth_type[i] = &frontalCoordinates;
            else tooth_type[i] = &molarCoordinates;
        }



        commonTexture = SpriteSheets::container().getCommonTexture();

}


QPixmap ToothPainter::paintToothStatus()
{
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    rotateByQuadrant(painter, coords->toothCrop.width(), coords->toothCrop.height(), currentIndex);

    painter.drawPixmap(0, 0, std::move(hyperdontic()));

    painter.drawPixmap(0, 0, std::move(lesionPixmap()));

    painter.drawPixmap(0, 0, std::move(perioPixmap()));

    painter.drawPixmap(0, 0, std::move(toothPixmap()));

    painter.drawPixmap(0, 0, std::move(endoPixmap()));

    //DRAWING THE POST
    if (tooth->post.exists())
    {
        
        painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop));
        painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop, QColor{ Qt::blue }, 0.1));
    }

    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, std::move(surfacePixmap()));
    painter.setOpacity(1);

    //DRAWING THE CROWN
    if (tooth->crown.exists()) {
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
    }




    return toothPx;
}

QPixmap* ToothPainter::getPixmap(const Tooth& tooth)
{
    int index = tooth.index;

    coords = tooth_type[index];
    this->tooth = &tooth;
    currentIndex = index;

    QPixmap pixmap(coords->toothCrop.width(), 746);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = container.getTexture(index, tooth.temporary.exists());
    
    
    painter.drawPixmap(0, 123, 
                       coords->toothCrop.width(), 
                       coords->toothCrop.height(),
                       std::move(paintToothStatus()));

    if (tooth.mobility.exists())
    {
        int height = 60; // for upper teeth;
        if (index > 15) height = 746-110; //for lower teeth

        painter.drawPixmap(0, height, std::move(mobilityPaint()));
    }
    
    int height = 0;
    if (index > 15) height = 746 - 50;

    painter.drawPixmap(0, height, std::move(toothNumber()));

    return new QPixmap(pixmap);
    
}

void ToothPainter::rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex)
{
    Quadrant q = utilities.getQuadrant(toothIndex);

    switch (q) //transforms the textures according the quadrant
    {
    case Quadrant::Second:
        painter.setTransform(QTransform::fromScale(-1, 1));
        painter.translate(-textureWidth, 0);
        break;

    case Quadrant::Third:
        painter.rotate(180);
        painter.translate(-textureWidth, -textureHeight);

        break;;

    case Quadrant::Fourth:
        painter.rotate(180);
        painter.setTransform(QTransform::fromScale(1, -1));
        painter.translate(0, -textureHeight);
        break;
    }
}

QPixmap ToothPainter::textureFormat(QRect crop, QColor color, double opacity)
{
    QPixmap pixmap = currentTexture->copy(crop);
    QPainter paint(&pixmap);
    paint.setOpacity(opacity);
    paint.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint.fillRect(0, 0, crop.width(), crop.height(), color);

    return pixmap;
}

QPixmap ToothPainter::textureFormat(QRect crop, double opacity)
{
    QPixmap pixmap(crop.width(), crop.height());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, currentTexture->copy(crop));

    return pixmap;
}

QPixmap ToothPainter::textureFormat(QRect crop)
{
    QPixmap pixmap = currentTexture->copy(crop);
    return pixmap;
}

QPixmap ToothPainter::toothPixmap()
{

    if (tooth->bridge.exists() && tooth->extraction.exists()) return QPixmap();

    QPixmap toothPixmap(coords->toothCrop.width(), coords->toothCrop.height());
    toothPixmap.fill(Qt::transparent);
    QPainter painter(&toothPixmap);

    if (tooth->extraction.exists() && !tooth->bridge.exists())            //drawing extracted tooth
    {
        painter.setOpacity(0.2);
        painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));
    }
    else if (tooth->root.exists())     //drawing root
    {
        painter.drawPixmap(0, 0, currentTexture->copy(coords->rootCrop));
    }
    else if (tooth->implant.exists()) //drawing implant
    {
        painter.drawPixmap(coords->implantPaint, *commonTexture, coords->implantCrop);
    }
    else {
        painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop)); //drawing normal tooth
    }

    return toothPixmap;
}

QPixmap ToothPainter::lesionPixmap()
{
    QPixmap lesionPixmap(coords->lesionCrop.width(), coords->lesionCrop.height());
    lesionPixmap.fill(Qt::transparent);
    QPainter painter(&lesionPixmap);

    painter.setOpacity(0.7);

    if (tooth->lesion.exists())
    {
        if (!tooth->implant.exists()) {
            painter.drawPixmap(0, 0, currentTexture->copy(coords->lesionCrop));
        }
        else {
            QRect iLesionCrop(120, 0, 120, 250);
            painter.drawPixmap(coords->implantPaint, *commonTexture, iLesionCrop);
        }
    }

    return lesionPixmap;
}

QPixmap ToothPainter::endoPixmap()
{
    if (tooth->implant.exists()) return QPixmap(); //no need to paint anything if it is an implant

    QPixmap rootCanal(currentTexture->copy(coords->endoCrop));

    QPainter painter(&rootCanal);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    
    QColor color;

    if (tooth->pulpitis.exists())
    {
        painter.setOpacity(0.5);
        color.setNamedColor("red");
    }
    else if (tooth->endo.exists())
    {
        painter.setOpacity(0.5);
        color.setNamedColor("blue");
    }
    else if(tooth->lesion.exists())
    {
        painter.setOpacity(0.6);
        color.setNamedColor("darkRed");
    }
    else
    {
        return QPixmap();
    }
    painter.fillRect(0, 0, coords->endoCrop.width(), coords->endoCrop.height(), color);

    return rootCanal;
}


QPixmap ToothPainter::perioPixmap()
{
    if (!tooth->periodontitis.exists()) return QPixmap();

    QPixmap periodontitis(coords->toothCrop.width(), coords->toothCrop.height());

    periodontitis.fill(Qt::transparent);

    if (!tooth->implant.exists()) {
        periodontitis = currentTexture->copy(coords->perioCrop);
        return periodontitis;
    }
 
    QPainter painter(&periodontitis);

    QRect perioImplantcrop(240, 0, 120, 250);
    painter.drawPixmap(coords->implantPaint, *commonTexture, perioImplantcrop);

    return periodontitis;
}

QPixmap ToothPainter::surfacePixmap()
{

    QPixmap surface(coords->toothCrop.width(), coords->toothCrop.height());
    surface.fill(Qt::transparent);
    QPainter surfPainter(&surface);

    for (int i = 0; i < 6; i++) //painting obturations
        if (tooth->obturation.exists(i))
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::blue, 1));

    for (int i = 0; i < 6; i++) //painting caries
        if (tooth->caries.exists(i))
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::red, 1));

    for (int i = 0; i < 6; i++) //painting secondary caries
        if (tooth->obturation.exists(i) && tooth->caries.exists(i))
            surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i]));

    return surface;
}

QPixmap ToothPainter::hyperdontic()
{
    if (!tooth->hyperdontic.exists()) return QPixmap();

    QPixmap toothPixmap(coords->toothCrop.width(), coords->toothCrop.height());
    toothPixmap.fill(Qt::transparent);
    QPainter painter(&toothPixmap);

    painter.drawPixmap(20, 0, currentTexture->copy(coords->toothCrop));

    return toothPixmap;
}

QPixmap ToothPainter::mobilityPaint()
{
    if (!tooth->mobility.exists()) return QPixmap();

    QString mobilityLabel;

    switch(tooth->mobility.degree)
    {
    case Degree::First:
        mobilityLabel = "🠘1🠚";
        break;
    case Degree::Second:
        mobilityLabel = "🠘2🠚";
        break;
    case Degree::Third:
        mobilityLabel = "🠘3🠚";
        break;
    }

    QPixmap mobility(coords->toothCrop.width(), 50);
    mobility.fill(Qt::transparent);
    QPainter painter(&mobility);
    
    QFont font{ "Arial", 28 };
    font.setBold(1);
    
    painter.setPen(QColor{ 255, 146, 148 });
    painter.setFont(font);
    painter.drawText(QRect{ 0,0,coords->toothCrop.width(), 50 }, Qt::AlignCenter, mobilityLabel);
    return mobility;
}

QPixmap ToothPainter::toothNumber()
{
    QPixmap numPixmap(coords->toothCrop.width(), 50);
    numPixmap.fill(Qt::transparent);
    QPainter painter(&numPixmap);

    QFont font{ "Arial", 28 };
    font.setBold(1);
    painter.setPen(Qt::gray);
    painter.setFont(font);

    int toothNumber = utilities.getToothNumber(currentIndex, tooth->temporary.exists());

    painter.drawText(QRect{ 0,0,coords->toothCrop.width(), 50 }, Qt::AlignCenter, QString::number(toothNumber));
    return numPixmap;
}



ToothPainter::~ToothPainter()
{



}










