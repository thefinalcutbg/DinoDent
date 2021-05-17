#include "ToothPainter.h"


ToothPainter::ToothPainter() :
    molarCoordinates(SpriteRectType::molar),
    premolarCoordinates(SpriteRectType::premolar),
    frontalCoordinates(SpriteRectType::frontal),
    temp_frontal(SpriteRectType::tempFrontal),
    temp_molar(SpriteRectType::tempMolar),
    currentTexture(NULL),
    coords(NULL)
{

    for (int i = 0; i < 32; i++) //mapping indexes to tooth types
    {
        if (i == 3 || i == 4 || i == 11 || i == 12)
        {
            tooth_type[i] = &premolarCoordinates;
            temp_tooth_type[i] = &temp_molar;

        }

        else if (i == 19 || i == 20 || i == 27 || i == 28)
        {
            tooth_type[i] = &premolarCoordinates;
            temp_tooth_type[i] = &temp_molar;
        }
        else if ((i > 2 && i < 13) || (i > 18 && i < 29))
        {
            tooth_type[i] = &frontalCoordinates;
            temp_tooth_type[i] = &temp_frontal;
        }
            
        else tooth_type[i] = &molarCoordinates;
    }
    
    commonTexture = SpriteSheets::container().getCommonTexture();
}

QPixmap* ToothPainter::paintTooth(const PaintHint& tooth)
{
    tooth.temp ?
    coords = temp_tooth_type[tooth.idx]
    :
    coords = tooth_type[tooth.idx];

    QPixmap pixmap(coords->toothCrop.width(), 746);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = container.getTexture(tooth.idx, tooth.temp);

    painter.drawPixmap(0, 123,
        coords->toothCrop.width(),
        coords->toothCrop.height(),
        std::move(returnPaintedTooth(tooth)));

    if (tooth.mobility)
    {
        int height = 60; // for upper teeth;
        if (tooth.idx > 15) height = 746 - 110; //for lower teeth

        

        painter.drawPixmap(0, height, std::move(mobilityPaint(tooth)));
    }

    int height = 0;
    if (tooth.idx > 15) height = 746 - 50;

    painter.drawPixmap(0, height, std::move(toothNumber(tooth)));

	return new QPixmap(pixmap);
}

QPixmap ToothPainter::returnPaintedTooth(const PaintHint& tooth)
{
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    rotateByQuadrant(painter, coords->toothCrop.width(), coords->toothCrop.height(), tooth.idx);

    if (tooth.lesion)
    {
        if (tooth.tooth != ToothHint::impl && tooth.tooth != ToothHint::impl_m)
        {
            painter.drawPixmap(0, 0, currentTexture->copy(coords->lesionCrop));
        }
        else
        {
            QRect iLesionCrop(120, 0, 120, 250);
            painter.drawPixmap(coords->implantPaint, *commonTexture, iLesionCrop);
        }
    }

    if (tooth.perio)
    {
        if (tooth.tooth != ToothHint::impl && tooth.tooth != ToothHint::impl_m)
        {
            painter.drawPixmap(0, 0, currentTexture->copy(coords->perioCrop));
        }
        else
        {
            QRect perioImplantcrop(240, 0, 120, 250);
            painter.drawPixmap(coords->implantPaint, *commonTexture, perioImplantcrop);
        }
    }

    auto& container = SpriteSheets::container();

    switch (tooth.tooth)    //drawing the tooth;
    {
        case ToothHint::none:
            break;
        case ToothHint::root:
            painter.drawPixmap(0, 0, currentTexture->copy(coords->rootCrop));
            break;
        case ToothHint::normal:
            painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));
            break;
        case ToothHint::extr:
            painter.setOpacity(0.2);
            painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));
            painter.setOpacity(1);
            break;
        case ToothHint::extr_m:
            painter.setOpacity(0.2);
            painter.drawPixmap(0, 0, currentTexture->copy(coords->toothCrop));
            painter.drawPixmap(0, 0, textureFormat(coords->toothCrop, Qt::green, 0.5));
            painter.setOpacity(1);
            currentTexture = container.getTexture(tooth.idx, tooth.temp);
            break;
        case ToothHint::impl_m:
            painter.drawPixmap(coords->implantPaint, *commonTexture, coords->implantCrop);
            painter.setOpacity(0.2);
            currentTexture = commonTexture; //fix texture format!!! (currently uses current texture!)
            painter.drawPixmap(coords->implantPaint, textureFormat(coords->implantCrop, Qt::green, 1));
            painter.setOpacity(1);
            currentTexture = container.getTexture(tooth.idx, tooth.temp);
            break;
        case ToothHint::impl:
            painter.drawPixmap(coords->implantPaint, *commonTexture, coords->implantCrop);
            break;
    }


    switch (tooth.endo) //drawing the endo
    {
        case EndoHint::none:
            break;
        case EndoHint::red:
            painter.drawPixmap(0, 0, textureFormat(coords->endoCrop, Qt::red, 0.5));
            break;
        case EndoHint::blue:
            painter.drawPixmap(0, 0, textureFormat(coords->endoCrop, Qt::blue, 0.5));
            break;
        case EndoHint::green:
            painter.drawPixmap(0, 0, textureFormat(coords->endoCrop, Qt::green, 0.5));
            break;
        case EndoHint::darkred:
            painter.drawPixmap(0, 0, textureFormat(coords->endoCrop, Qt::darkRed, 0.6));
            break;
    }

    switch (tooth.post) //drawing the post
    {
    case PostHint::none: 
        break;
    case PostHint::blue:
            painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop));
            painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop, QColor{ Qt::blue }, 0.3));
        break;
    case PostHint::green:
            painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop));
            painter.drawPixmap(coords->postPos, textureFormat(coords->postCrop, QColor{ Qt::green }, 0.3));
        break;


    }

    //surfaces:
    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, drawSurfaces(tooth));
    painter.setOpacity(1);

    //the crown:

    switch (tooth.prostho)
    {
    case ProsthoHint::none:
        break;
    case ProsthoHint::crown:
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
        break;
    case ProsthoHint::crown_green:
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop));
        painter.drawPixmap(coords->crownPos, textureFormat(coords->crownCrop, Qt::green, 0.3));
        break;
    }

    return QPixmap(toothPx);
}

QPixmap ToothPainter::drawSurfaces(const PaintHint& tooth)
{
    QPixmap surface(coords->toothCrop.width(), coords->toothCrop.height());
    surface.fill(Qt::transparent);
    QPainter surfPainter(&surface);

    QPixmap outlinedSurface(coords->toothCrop.width(), coords->toothCrop.height());
    outlinedSurface.fill(Qt::transparent);
    QPainter outlinePainter(&outlinedSurface);



    for (int i = 0; i < tooth.surfaces.size(); i++) //drawing the surfaces;
    {
        if (!tooth.surfaces[i].outline)
        {
            switch (tooth.surfaces[i].color)
            {
            case SurfaceColor::blue:
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::blue, 1));
                break;
            case SurfaceColor::red:
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::red, 1));
                break;
            case SurfaceColor::green:
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::green, 1));
                break;
            default:
                break;
            }
        }
        else
        {
            switch (tooth.surfaces[i].color)
            {
            case SurfaceColor::blue:

                outlinePainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::blue, 1));
                break;
            case SurfaceColor::green:
                outlinePainter.drawPixmap(coords->surfPos[i], textureFormat(coords->surfCrop[i], Qt::green, 1));
                break;
            default:
                break;
            }
        }
    }

    QPixmap pixmap_of_united_surfaces(coords->toothCrop.width(), coords->toothCrop.height());
    pixmap_of_united_surfaces.fill(Qt::transparent);
    QPainter painter_of_united_surfaces(&pixmap_of_united_surfaces);
    painter_of_united_surfaces.drawPixmap(0, 0, surface);
    painter_of_united_surfaces.drawPixmap(0, 0, textureOutline(outlinedSurface, Qt::red));

    return pixmap_of_united_surfaces;

}

QPixmap ToothPainter::mobilityPaint(const PaintHint& tooth)
{
    QString mobilityLabel = "🠘" + QString::number(tooth.mobility) + "🠚";

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

QPixmap ToothPainter::toothNumber(const PaintHint& tooth)
{

    QPixmap numPixmap(coords->toothCrop.width(), 50);
    numPixmap.fill(Qt::transparent);
    QPainter painter(&numPixmap);

    QFont font{ "Arial", 28 };
    font.setBold(1);
    painter.setPen(Qt::gray);
    painter.setFont(font);

    painter.drawText(QRect{ 0,0,coords->toothCrop.width(), 50 }, Qt::AlignCenter, QString::number(tooth.num));
    return numPixmap;

}

void ToothPainter::rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, int toothIndex)
{

    if (toothIndex < 8)
    {
        return;
    }
    else if (toothIndex < 16) //transforms the textures according the quadrant
    {
        painter.setTransform(QTransform::fromScale(-1, 1));
        painter.translate(-textureWidth, 0);
    }
    else if (toothIndex < 24)
    {
        painter.rotate(180);
        painter.translate(-textureWidth, -textureHeight);
    }
    else
    {
        painter.rotate(180);
        painter.setTransform(QTransform::fromScale(1, -1));
        painter.translate(0, -textureHeight);
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

QPixmap ToothPainter::textureOutline(const QPixmap& src, QColor borderColor)
{
    QPixmap outline_px(src);

    QPainter painter(&outline_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

    QGraphicsPixmapItem temp_pixmap_item(outline_px);
    auto path = temp_pixmap_item.shape();

    QPen pen(Qt::red, 20, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
   
    painter.setPen(pen);
    painter.drawPath(path);

    return outline_px;
}
