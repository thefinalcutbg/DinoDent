#include "ToothPainter.h"


ToothPainter::ToothPainter() :
    currentTexture(nullptr),
    coords(nullptr),
    showLingual(false)
{

    
}

QPixmap* ToothPainter::paintTooth(const ToothPaintHint& tooth)
{
    int pxHeight = showLingual ? 1106 : 746;


    coords = &SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);

    QPixmap pixmap(coords->toothCrop.width(), pxHeight);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = &container.getTexturePack(tooth.idx, tooth.temp);

    int yPos = 123;



    if (tooth.impacted && !tooth.dns) //hyperdontic edge case should be considered!
        yPos = tooth.idx > 15 ? 200 : 40;

        if (tooth.mobility) {
            int height = tooth.idx > 15 ? pxHeight - 110 : 60;
            painter.drawPixmap(0, height, mobilityPaint(tooth));
        }

    if (tooth.frac) {
        int height = tooth.idx > 15 ? 50 : 650;
        painter.drawPixmap(0, height, fracturePaint(tooth));
    }

    int height = tooth.idx > 15 ? pxHeight - 50 : 0;

    painter.drawPixmap(0, height, toothNumber(tooth));


    rotateByQuadrant(painter, coords->toothCrop.width(), 750, tooth.idx);
   
    if (showLingual)
    {
        if (tooth.idx < 16)
            painter.drawPixmap(0, yPos, returnPaintedTooth(tooth));
        else
            painter.drawPixmap(0, yPos*(-2), returnPaintedTooth(tooth));
        return new QPixmap(pixmap);
    }

    painter.drawPixmap(QRect(0, yPos,
        coords->toothCrop.width(), 500),
        returnPaintedTooth(tooth), coords->BuccalOcclusalCrop);

    return new QPixmap(pixmap);
	
}

QPixmap ToothPainter::getPixmap(const ToothPaintHint& tooth)
{
    coords = &SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);

    QPixmap pixmap(coords->toothCrop.width(), 746);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    auto& container = SpriteSheets::container();
    currentTexture = &container.getTexturePack(tooth.idx, tooth.temp);

    int yPos = 123;

    if (tooth.impacted) //hyperdontic edge case should be considered!
    {
        if (tooth.idx < 16) yPos = 23;
        else yPos = 223;
    }

    painter.drawPixmap(0, yPos,
        coords->toothCrop.width(),
        coords->toothCrop.height(),
        returnPaintedTooth(tooth));

    if (tooth.mobility)
    {
        yPos = 60; // for upper teeth;
        if (tooth.idx > 15) yPos = 746 - 110; //for lower teeth

        painter.drawPixmap(0, yPos, mobilityPaint(tooth));
    }

    if (tooth.frac)
    {
        yPos = 650;
        if (tooth.idx > 15) yPos = 50;
        painter.drawPixmap(0, yPos, fracturePaint(tooth));
    }

    yPos = 0;
    if (tooth.idx > 15) yPos = 746 - 50;

    painter.drawPixmap(0, yPos, toothNumber(tooth));

    return pixmap;
}

QPixmap ToothPainter::returnPaintedTooth(const ToothPaintHint& tooth)
{
    QPoint point(0, 0);

    QPixmap toothPx(coords->toothCrop.width(), coords->toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    

    if (tooth.lesion)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(0, 0, *currentTexture->lesion);
            
        }
        else
        {
            painter.drawPixmap(coords->implantPaint, *currentTexture->lesionImplant);
        }
    }

    if (tooth.perio)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(0, 0, *currentTexture->perio);
        }
        else
        {
           
            painter.drawPixmap(coords->implantPaint, *currentTexture->perioImplant);
        }
    }

    auto& container = SpriteSheets::container();

    switch (tooth.tooth)    //drawing the tooth;
    {
        case ToothTextureHint::none:
            break;
        case ToothTextureHint::root:
            painter.drawPixmap(0, 0, *currentTexture->root);
            break;
        case ToothTextureHint::normal:
            painter.drawPixmap(0, 0, *currentTexture->tooth);
            break;
        case ToothTextureHint::extr:
            painter.setOpacity(0.2);
            painter.drawPixmap(0, 0, *currentTexture->tooth);
            painter.setOpacity(1);
            break;
        case ToothTextureHint::extr_m:
            painter.setOpacity(0.2);
            painter.drawPixmap(0, 0, *currentTexture->tooth);
            painter.drawPixmap(0, 0, textureFormat(*currentTexture->tooth, Qt::green, 0.3));
            painter.setOpacity(1);
            currentTexture = &container.getTexturePack(tooth.idx, tooth.temp);
            break;

        case ToothTextureHint::impl_m:
            painter.drawPixmap(coords->implantPaint, *currentTexture->implant);
            painter.setOpacity(0.2);
            painter.drawPixmap(coords->implantPaint, textureFormat(*currentTexture->implant, Qt::green, 1));
            painter.setOpacity(1);
            currentTexture = &container.getTexturePack(tooth.idx, tooth.temp);
            break;
        case ToothTextureHint::impl:
            painter.drawPixmap(coords->implantPaint, *currentTexture->implant);
            break;
    }


    switch (tooth.endo) //drawing the endo
    {
        case EndoHint::none:
            break;
        case EndoHint::red:
            painter.drawPixmap(0, 0, textureFormat(*currentTexture->endo, Qt::red, 0.5));
            break;
        case EndoHint::blue:
            painter.drawPixmap(0, 0, textureFormat(*currentTexture->endo, Qt::blue, 0.5));
            break;
        case EndoHint::green:
            painter.drawPixmap(0, 0, textureFormat(*currentTexture->endo, Qt::green, 0.5));
            break;
        case EndoHint::darkred:
            painter.drawPixmap(0, 0, textureFormat(*currentTexture->endo, Qt::darkRed, 0.6));
            break;
    }

    switch (tooth.post) //drawing the post
    {
    case PostHint::none: 
        break;
    case PostHint::blue:
            painter.drawPixmap(coords->postPos, *currentTexture->post);
            painter.drawPixmap(coords->postPos, textureFormat(*currentTexture->post, QColor{ Qt::blue }, 0.3));
        break;
    case PostHint::green:
            painter.drawPixmap(coords->postPos, *currentTexture->post);
            painter.drawPixmap(coords->postPos, textureFormat(*currentTexture->post, QColor{ Qt::green }, 0.3));
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
        painter.drawPixmap(coords->crownPos, *currentTexture->crown);
        break;
    case ProsthoHint::crown_green:
        painter.drawPixmap(coords->crownPos, *currentTexture->crown);
        painter.drawPixmap(coords->crownPos, textureFormat(*currentTexture->crown, Qt::green, 0.3));
        break;
    }

    if (tooth.impacted || showLingual) painter.eraseRect(0, 360, toothPx.width(), 140);

    return toothPx;
}

QPixmap ToothPainter::drawSurfaces(const ToothPaintHint& tooth)
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
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(*currentTexture->surfaces[i], Qt::blue, 1));
                break;
            case SurfaceColor::red:
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(*currentTexture->surfaces[i], Qt::red, 1));
                break;
            case SurfaceColor::green:
                surfPainter.drawPixmap(coords->surfPos[i], textureFormat(*currentTexture->surfaces[i], Qt::green, 1));
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

                outlinePainter.drawPixmap(coords->surfPos[i], textureFormat(*currentTexture->surfaces[i], Qt::blue, 1));
                break;
            case SurfaceColor::green:
                outlinePainter.drawPixmap(coords->surfPos[i], textureFormat(*currentTexture->surfaces[i], Qt::green, 1));
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

QPixmap ToothPainter::mobilityPaint(const ToothPaintHint& tooth)
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

QPixmap ToothPainter::fracturePaint(const ToothPaintHint& tooth)
{
    QPixmap fracture(coords->toothCrop.width(), 55);
    fracture.fill(Qt::transparent);

    QPainter painter(&fracture);
    painter.setOpacity(0.5);
    QRect rect(coords->toothCrop.width() / 2 - 25, 2, 50, 50);

    //painter.fillRect(rect, Qt::GlobalColor::white);

    QPen pen(Qt::red);
    pen.setWidth(5);
    painter.setPen(pen);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(rect);

    QFont font{ "Arial", 28 };
    font.setBold(1);
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, "F");
    return fracture;
    
}

QPixmap ToothPainter::toothNumber(const ToothPaintHint& tooth)
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


QPixmap ToothPainter::textureFormat(const QPixmap& px, QColor color, double opacity)
{
    QPixmap pixmap = px;
    QPainter paint(&pixmap);
    paint.setOpacity(opacity);
    paint.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint.fillRect(0, 0, px.width(), px.height(), color);

    return pixmap;
}

QPixmap ToothPainter::textureFormat(const QPixmap& px, double opacity)
{
    QPixmap pixmap(px);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, px);

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

    QPen pen(Qt::red, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
   
    painter.setPen(pen);
    painter.drawPath(path);

    return outline_px;
}
