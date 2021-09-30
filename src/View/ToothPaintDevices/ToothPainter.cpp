#include "ToothPainter.h"
#include "SpriteSheets.h"
#include "PaintHint.h"
#include <QGraphicsPixmapItem>
#include <QPainter>


inline QPixmap textureFormat(const QPixmap& px, QColor color, double opacity)
{
    QPixmap pixmap = px;
    QPainter paint(&pixmap);
    paint.setOpacity(opacity);
    paint.setCompositionMode(QPainter::CompositionMode_SourceIn);
    paint.fillRect(0, 0, px.width(), px.height(), color);

    return pixmap;
}

inline QPixmap textureFormat(const QPixmap& px, double opacity)
{
    QPixmap pixmap(px);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setOpacity(opacity);
    painter.drawPixmap(0, 0, px);

    return pixmap;
}


inline QPixmap textureOutline(const QPixmap& src, QColor borderColor)
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

inline QPixmap getSurfaceTexture(const ToothPaintHint& tooth)
{
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap surface(coords.toothCrop.width(), coords.toothCrop.height());
    surface.fill(Qt::transparent);
    QPainter surfPainter(&surface);

    QPixmap outlinedSurface(coords.toothCrop.width(), coords.toothCrop.height());
    outlinedSurface.fill(Qt::transparent);
    QPainter outlinePainter(&outlinedSurface);

    for (int i = 0; i < tooth.surfaces.size(); i++) //drawing the surfaces;
    {
        if (!tooth.surfaces[i].outline)
        {
            switch (tooth.surfaces[i].color)
            {
            case SurfaceColor::blue:
                surfPainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], Qt::blue, 1));
                break;
            case SurfaceColor::red:
                surfPainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], Qt::red, 1));
                break;
            case SurfaceColor::green:
                surfPainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], Qt::green, 1));
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

                outlinePainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], Qt::blue, 1));
                break;
            case SurfaceColor::green:
                outlinePainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], Qt::green, 1));
                break;
            default:
                break;
            }
        }
    }

    QPixmap pixmap_of_united_surfaces(coords.toothCrop.width(), coords.toothCrop.height());
    pixmap_of_united_surfaces.fill(Qt::transparent);
    QPainter painter_of_united_surfaces(&pixmap_of_united_surfaces);
    painter_of_united_surfaces.drawPixmap(0, 0, surface);
    painter_of_united_surfaces.drawPixmap(0, 0, textureOutline(outlinedSurface, Qt::red));

    return pixmap_of_united_surfaces;

}

inline QPixmap getToothPixmap(const ToothPaintHint& tooth)
{
    QPoint point(0, 0);

    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap toothPx(coords.toothCrop.width(), coords.toothCrop.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);



    if (tooth.lesion)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(0, 0, *texturePack.lesion);

        }
        else
        {
            painter.drawPixmap(coords.implantPaint, *texturePack.lesionImplant);
        }
    }

    if (tooth.perio)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(0, 0, *texturePack.perio);
        }
        else
        {

            painter.drawPixmap(coords.implantPaint, *texturePack.perioImplant);
        }
    }

    auto& container = SpriteSheets::container();

    switch (tooth.tooth)    //drawing the tooth;
    {
    case ToothTextureHint::none:
        break;
    case ToothTextureHint::root:
        painter.drawPixmap(0, 0, *texturePack.root);
        break;
    case ToothTextureHint::normal:
        painter.drawPixmap(0, 0, *texturePack.tooth);
        break;
    case ToothTextureHint::extr:
        painter.setOpacity(0.2);
        painter.drawPixmap(0, 0, *texturePack.tooth);
        painter.setOpacity(1);
        break;
    case ToothTextureHint::extr_m:
        painter.setOpacity(0.2);
        painter.drawPixmap(0, 0, *texturePack.tooth);
        painter.drawPixmap(0, 0, textureFormat(*texturePack.tooth, Qt::green, 0.3));
        painter.setOpacity(1);
        break;

    case ToothTextureHint::impl_m:
        painter.drawPixmap(coords.implantPaint, *texturePack.implant);
        painter.setOpacity(0.2);
        painter.drawPixmap(coords.implantPaint, textureFormat(*texturePack.implant, Qt::green, 1));
        painter.setOpacity(1);
        break;
    case ToothTextureHint::impl:
        painter.drawPixmap(coords.implantPaint, *texturePack.implant);
        break;
    }


    switch (tooth.endo) //drawing the endo
    {
    case EndoHint::none:
        break;
    case EndoHint::red:
        painter.drawPixmap(0, 0, textureFormat(*texturePack.endo, Qt::red, 0.3));
        break;
    case EndoHint::blue:
        painter.drawPixmap(0, 0, textureFormat(*texturePack.endo, Qt::blue, 0.3));
        break;
    case EndoHint::green:
        painter.drawPixmap(0, 0, textureFormat(*texturePack.endo, Qt::green, 0.3));
        break;
    case EndoHint::darkred:
        painter.drawPixmap(0, 0, textureFormat(*texturePack.endo, Qt::darkRed, 0.3));
        break;
    }

    switch (tooth.post) //drawing the post
    {
    case PostHint::none:
        break;
    case PostHint::blue:
        painter.drawPixmap(coords.postPos, *texturePack.post);
        painter.drawPixmap(coords.postPos, textureFormat(*texturePack.post, QColor{ Qt::blue }, 0.3));
        break;
    case PostHint::green:
        painter.drawPixmap(coords.postPos, *texturePack.post);
        painter.drawPixmap(coords.postPos, textureFormat(*texturePack.post, QColor{ Qt::green }, 0.3));
        break;


    }

    //surfaces:

    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, getSurfaceTexture(tooth));
    painter.setOpacity(1);

    //the crown:

    switch (tooth.prostho)
    {
    case ProsthoHint::none:
        break;
    case ProsthoHint::crown:
        painter.drawPixmap(coords.crownPos, *texturePack.crown);
        break;
    case ProsthoHint::crown_green:
        painter.drawPixmap(coords.crownPos, *texturePack.crown);
        painter.drawPixmap(coords.crownPos, textureFormat(*texturePack.crown, Qt::green, 0.3));
        break;
    }

    if (tooth.impacted) painter.eraseRect(0, 360, toothPx.width(), 140);

    return toothPx;
}

inline void drawMobilityLabel(const ToothPaintHint& tooth, QPixmap& pixmap)
{
	if (!tooth.mobility) return;
    
	QString mobilityLabel = "🠘" + QString::number(tooth.mobility) + "🠚";

	QPainter painter(&pixmap);

	QFont font{ "Arial", 28 };
	font.setBold(1);

	
    int yPos = tooth.idx > 15 ? pixmap.height() - 123 : 50;

	painter.setPen(QColor{ 255, 146, 148 });
	painter.setFont(font);
	painter.drawText(QRect{ 0, yPos, pixmap.width(), 50}, Qt::AlignCenter, mobilityLabel);

}


inline void drawFractureLabel(const ToothPaintHint& tooth, QPixmap& pixmap)
{
	if (!tooth.frac) return;

    int yPos = tooth.idx > 15 ? 20 : pixmap.height() - 60;

	QPainter painter(&pixmap);
	painter.setOpacity(0.5);
	QRect rect(0, yPos, pixmap.width(), 50);

	QPen pen(Qt::red);
	pen.setWidth(5);
	painter.setPen(pen);

	painter.setRenderHint(QPainter::Antialiasing);
	painter.drawEllipse(QRect(pixmap.width()/2 - 25, yPos, 50, 50));

	QFont font{ "Arial", 28 };
	font.setBold(1);
	painter.setFont(font);
	painter.drawText(rect, Qt::AlignCenter, "F");

}

inline void drawToothNumberLabel(const ToothPaintHint& tooth, QPixmap& pixmap)
{
	QPainter painter(&pixmap);

	int yPos = tooth.idx > 15 ? pixmap.height() - 50 : 0;

	QFont font{ "Arial", 28 };
	font.setBold(1);
	painter.setPen(Qt::gray);
	painter.setFont(font);

	painter.drawText(QRect{ 0,yPos,pixmap.width(), 50 }, Qt::AlignCenter, QString::number(tooth.num));
}


void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, Quadrant quadrant)
{

    switch (quadrant)
    {
    case Quadrant::First:
        return;
    case Quadrant::Second:
        painter.setTransform(QTransform::fromScale(-1, 1));
        painter.translate(-textureWidth, 0);
        return;
    case Quadrant::Third:
        painter.rotate(180);
        painter.translate(-textureWidth, -textureHeight);
        break;
    case Quadrant::Fourth:
        painter.rotate(180);
        painter.setTransform(QTransform::fromScale(1, -1));
        painter.translate(0, -textureHeight);
        break;
    }

}

QPixmap ToothPainter::getBuccalOcclusal(const ToothPaintHint& tooth)
{
	constexpr int pixmapHeight = 746;

	auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
	auto& currentTexture = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

	QPixmap pixmap(coords.toothCrop.width(), pixmapHeight);
	pixmap.fill(Qt::transparent);


	drawMobilityLabel(tooth, pixmap);
	drawFractureLabel(tooth, pixmap);
	drawToothNumberLabel(tooth, pixmap);

	QPainter painter(&pixmap);

    constexpr int toothYPosition = 123;

    rotateByQuadrant(painter, coords.toothCrop.width(), pixmapHeight, ToothUtils::getQuadrant(tooth.idx));

    if (tooth.impacted)
    {
        int yPos = tooth.idx > 15 ? 200 : 40;
        painter.drawPixmap(QRect(0, 40, coords.toothCrop.width(), 360), getToothPixmap(tooth), coords.buccalImpactCrop);
    }
    else
    {
        painter.drawPixmap(QRect(0, toothYPosition,
            coords.toothCrop.width(), 500),
            getToothPixmap(tooth), coords.BuccalOcclusalCrop);
    }

    return pixmap;
}

QPixmap ToothPainter::getBuccalLingual(const ToothPaintHint& tooth)
{
    constexpr int pixmapHeight = 1106;

    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& currentTexture = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap pixmap(coords.toothCrop.width(), pixmapHeight);
    pixmap.fill(Qt::transparent);


    drawMobilityLabel(tooth, pixmap);
    drawFractureLabel(tooth, pixmap);
    drawToothNumberLabel(tooth, pixmap);

    QPainter painter(&pixmap);

    constexpr int toothYPosition = 123;

    rotateByQuadrant(painter, coords.toothCrop.width(), pixmapHeight, ToothUtils::getQuadrant(tooth.idx));

    if (tooth.impacted)
    {
        QPixmap normalTooth = getToothPixmap(tooth);
        painter.drawPixmap(QRect(0, 40, coords.toothCrop.width(), 320), normalTooth, coords.buccalImpactCrop);
        painter.drawPixmap(QRect(0, 750, coords.toothCrop.width(), 320), normalTooth, coords.lingualImpactCrop);
    }
    else
    {
        painter.drawPixmap(0, 123,
            getToothPixmap(tooth));
        painter.eraseRect(0, 123 + 360, coords.toothCrop.width(), 140);
    }

    return pixmap;
}


QPixmap ToothPainter::getOcclusal(const ToothPaintHint& tooth)
{
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& currentTexture = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    if (tooth.tooth != ToothTextureHint::normal) return QPixmap();

    QPoint point(0, 0);

    QPixmap toothPx(coords.toothCrop.width(), coords.toothCrop.height());
    toothPx.fill(Qt::transparent);

    QPainter toothPainter(&toothPx);

    toothPainter.drawPixmap(0, 0, *currentTexture.tooth);

    toothPainter.setOpacity(0.35);
    toothPainter.drawPixmap(0, 0, getSurfaceTexture(tooth));

    toothPainter.setOpacity(0.35);
    if (tooth.prostho != ProsthoHint::none)
    {
        if (tooth.prostho == ProsthoHint::crown || tooth.prostho == ProsthoHint::bridge)
        {
            toothPainter.drawPixmap(coords.crownPos, *currentTexture.crown);
        }
        else
        {
            toothPainter.drawPixmap(coords.crownPos, *currentTexture.crown);
            toothPainter.drawPixmap(coords.crownPos, textureFormat(*currentTexture.crown, Qt::green, 0.3));
        }

    }

    //transforming:

    QPixmap pixmap(150, 150);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);

    rotateByQuadrant(painter, 150, 150, ToothUtils::getQuadrant(tooth.idx));

    painter.drawPixmap(QRect(0, 0, 150, 150), toothPx, coords.SurfacePanelCrop);

    return pixmap;
}
