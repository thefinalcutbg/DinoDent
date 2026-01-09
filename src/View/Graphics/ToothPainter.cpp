#include "ToothPainter.h"
#include "SpriteSheets.h"
#include "PaintHint.h"
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QtGlobal>

const QColor BLUE = QColor(0, 100, 255);
const QColor GREEN = QColor(0, 255, 110);

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


QPixmap getBridgeTexture(const ToothPaintHint& tooth)
{
    //auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

   // auto& raw = texturePack.rawBridge;

    int height = texturePack.bridgeConnected->height();
    int width = texturePack.bridgeConnected->width();

    QPixmap result(width, height);
    result.fill(Qt::transparent);
    QPainter painter(&result);

    switch (tooth.bridgePos)
    {
    case BridgeTerminal::Center:
        painter.drawPixmap(QRect(0, 0, width, height), *texturePack.bridgeConnected, QRect(0, 0, width, height));
        break;
    case BridgeTerminal::Distal:
    {
        painter.drawPixmap(QRect(0, 0, width / 2, height), *texturePack.bridgeSeparated, QRect(0, 0, width / 2, height));
        painter.drawPixmap(QRect(width / 2, 0, width / 2, height), *texturePack.bridgeConnected, QRect(width / 2, 0, width / 2, height));
        break;
    }
    case BridgeTerminal::Medial:
    {
        painter.drawPixmap(QRect(0, 0, width / 2, height), *texturePack.bridgeConnected, QRect(0, 0, width / 2, height));
        painter.drawPixmap(QRect(width / 2, 0, width / 2, height), *texturePack.bridgeSeparated, QRect(width/2, 0, width / 2, height));
        break;
    }
    }

    return result;

}

QPixmap getSplintRect(const ToothPaintHint& tooth)
{
    if (tooth.prostho != ProsthoHint::splint &&
        tooth.prostho != ProsthoHint::splint_green) return QPixmap();

    //auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    constexpr int height = 150;
    constexpr int splintHeight = 30;
    int width = texturePack.bridgeSeparated->width();

    QPixmap result(width, height);
    result.fill(Qt::transparent);
    QPainter painter(&result);

    QColor color(tooth.prostho == ProsthoHint::splint_green ? GREEN : BLUE);

    QBrush brush(color);
    painter.setBrush(brush);

    switch (tooth.bridgePos)
    {
    case BridgeTerminal::Center:
        painter.fillRect(QRect(0, 60, width, splintHeight), color);
        break;
    case BridgeTerminal::Distal:
        painter.fillRect(QRect(width/2, 60, (width / 2), splintHeight), color);
        painter.drawEllipse(QPoint(width / 2, 75), splintHeight / 2, splintHeight / 2);
        break;
    case BridgeTerminal::Medial:
        painter.fillRect(QRect(0, 60, width / 2, splintHeight), color);
        painter.drawEllipse(QPoint(width / 2, 75), splintHeight / 2, splintHeight / 2);
        break;

    }

    return result;
}

inline QPixmap getDenture(const ToothPaintHint& tooth) {

    if (tooth.prostho != ProsthoHint::denture &&
        tooth.prostho != ProsthoHint::denture_green) return {};
    
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);
    
    QPixmap denture(coords.toothRect.width(), coords.toothRect.height());
    denture.fill(Qt::transparent);
    
    QPainter painter(&denture);

    painter.drawPixmap(coords.toothRect, *texturePack.denture);
    painter.drawPixmap(coords.crownRect, *texturePack.falseTooth);

    return denture;
}

QPixmap textureOutline(const QPixmap& src, QColor color)
{
    QPixmap outline_px(src);

    QPainter painter(&outline_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);

    QGraphicsPixmapItem temp_pixmap_item(outline_px);
    auto path = temp_pixmap_item.shape();

    QPen pen(color, 15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    painter.setPen(pen);
    painter.drawPath(path);

    return outline_px;
}

QPixmap textureStripe(const QPixmap& src)
{
    QPixmap outline_px(src);

    static QPixmap stripes = QPixmap(":/tooth/tooth_stripes.png");

    QPainter painter(&outline_px);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);

    painter.drawPixmap(outline_px.rect(), stripes);

    return outline_px;
}

QPixmap getSurfaceTexture(const ToothPaintHint& tooth)
{
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap surface(coords.toothRect.width(), coords.toothRect.height());
    surface.fill(Qt::transparent);
    QPainter surfPainter(&surface);

    std::map<SurfaceColor, QColor> colorToQColorMap = {
        {SurfaceColor::none, QColor()},
        {SurfaceColor::blue, BLUE},
        {SurfaceColor::green, GREEN},
        {SurfaceColor::red, Qt::red},
        {SurfaceColor::orange, QColor(225,128,0)}
    };

    std::map<SurfaceColor, std::vector<int>> outlineColorSurfaceMap; //stores the different outlines
    std::map<SurfaceColor, std::vector<int>> stripeColorSurfaceMap; //stores the different stripes

    for (size_t i = 0; i < tooth.surfaces.size(); i++) //drawing the surfaces;
    {
        if (tooth.surfaces[i].color != SurfaceColor::none)
        {
            auto& qtColor = colorToQColorMap[tooth.surfaces[i].color];

            surfPainter.drawPixmap(coords.surfPos[i], textureFormat(*texturePack.surfaces[i], qtColor, 1));
        }

        if (tooth.surfaces[i].outline != SurfaceColor::none) {
            outlineColorSurfaceMap[tooth.surfaces[i].outline].push_back(i);
        }

        if (tooth.surfaces[i].stripes != SurfaceColor::none) {
            stripeColorSurfaceMap[tooth.surfaces[i].stripes].push_back(i);
        }
    }

    QPixmap endResult(coords.toothRect.width(), coords.toothRect.height());
    endResult.fill(Qt::transparent);
    QPainter endResultPainter(&endResult);
    endResultPainter.drawPixmap(0, 0, surface);

    //drawing the outlines
    for (auto& [color, surfaceList] : outlineColorSurfaceMap) {

        QPixmap outlinedSurface(coords.toothRect.width(), coords.toothRect.height());
        outlinedSurface.fill(Qt::transparent);
        QPainter outlinePainter(&outlinedSurface);

        for (auto surface : surfaceList) {

            auto& insideColor = colorToQColorMap[tooth.surfaces[surface].color];

            outlinePainter.drawPixmap(coords.surfPos[surface], textureFormat(*texturePack.surfaces[surface], insideColor, 1));
        }

        endResultPainter.drawPixmap(0, 0, textureOutline(outlinedSurface, colorToQColorMap[color]));
    }

    //drawing the stripes
    for (auto& [color, surfaceList] : stripeColorSurfaceMap) {

        QPixmap stripedSurface(coords.toothRect.width(), coords.toothRect.height());
        stripedSurface.fill(Qt::transparent);
        QPainter stripePainter(&stripedSurface);

        for (auto surface : surfaceList) {

            auto& stripeColor = colorToQColorMap[tooth.surfaces[surface].stripes];

            stripePainter.drawPixmap(coords.surfPos[surface], textureFormat(*texturePack.surfaces[surface], stripeColor, 1));
        }

        endResultPainter.drawPixmap(0, 0, textureStripe(stripedSurface));
    }

    return endResult;
}

inline QPixmap getTooth(const ToothPaintHint& tooth) {

    //QPoint point(0, 0);

    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap toothPx(coords.toothRect.width(), coords.toothRect.height());
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    //drawing the lesion:

    if (tooth.lesion)
    {
            painter.drawPixmap(0, 0, *texturePack.lesion);
    }

    //drawing the perio:

    if (tooth.perio)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(0, 0, *texturePack.perio);
        }
        else
        {

            painter.drawPixmap(coords.implantPos, *texturePack.perioImplant);
        }
    }

    if (tooth.calculus)
    {
        if (tooth.tooth != ToothTextureHint::impl && tooth.tooth != ToothTextureHint::impl_m)
        {
            painter.drawPixmap(coords.crownRect, *texturePack.calculus);
        }
        else
        {
            painter.drawPixmap(coords.implantPos, *texturePack.calculusImplant);
        }
    }

    //drawing the tooth:

    switch (tooth.tooth)
    {
    case ToothTextureHint::none:
        break;
    case ToothTextureHint::root:
        painter.drawPixmap(0, 0, *texturePack.root);
        break;
    case ToothTextureHint::normal:
            painter.drawPixmap(0, 0, *texturePack.tooth);
        break;
    case ToothTextureHint::unknown:
        painter.setOpacity(0.6);
        painter.drawPixmap(0, 0, *texturePack.tooth);
        painter.setOpacity(1);
        break;
    case ToothTextureHint::extr:
        painter.setOpacity(0.1);
        painter.drawPixmap(0, 0, *texturePack.tooth);
        painter.setOpacity(1);
        break;
    case ToothTextureHint::extr_m:
        painter.setOpacity(0.1);
        painter.drawPixmap(0, 0, *texturePack.tooth);
        painter.drawPixmap(0, 0, textureFormat(*texturePack.tooth, GREEN, 0.3));
        painter.setOpacity(1);
        break;

    case ToothTextureHint::impl_m:
        painter.drawPixmap(coords.implantPos, *texturePack.implant);
        painter.setOpacity(0.2);
        painter.drawPixmap(coords.implantPos, textureFormat(*texturePack.implant, GREEN, 1));
        painter.setOpacity(1);
        break;
    case ToothTextureHint::impl:
        painter.drawPixmap(coords.implantPos, *texturePack.implant);
        break;
    }


    QPixmap endo(coords.toothRect.width(), coords.toothRect.height()); 
    endo.fill(Qt::transparent);

    if (tooth.resorption) {
        endo = *texturePack.resorption;
    }

    if (tooth.endo != EndoHint::none) {
        QPainter canalPainter(&endo);
        canalPainter.drawPixmap(endo.rect(), *texturePack.endo);
    }
    
    switch (tooth.endo) //drawing the endo
    {
    case EndoHint::none:
        painter.drawPixmap(0, 0, textureFormat(endo, Qt::red, 0.3));
        break;
    case EndoHint::red:
        painter.drawPixmap(0, 0, textureFormat(endo, Qt::red, 0.3));
        break;
    case EndoHint::blue:
        painter.drawPixmap(0, 0, textureFormat(endo, BLUE, 0.3));
        break;
    case EndoHint::green:
        painter.drawPixmap(0, 0, textureFormat(endo, GREEN, 0.3));
        break;
    case EndoHint::darkred:
        painter.drawPixmap(0, 0, textureFormat(endo, Qt::darkRed, 0.3));
        break;
    }

    switch (tooth.post) //drawing the post
    {
    case PostHint::none:
        break;
    case PostHint::blue:
        painter.drawPixmap(0, 0, *texturePack.post);
        painter.drawPixmap(0, 0, textureFormat(*texturePack.post, QColor{ BLUE }, 0.5));
        break;
    case PostHint::green:
        painter.drawPixmap(0, 0, *texturePack.post);
        painter.drawPixmap(0, 0, textureFormat(*texturePack.post, QColor{ GREEN }, 0.5));
        break;


    }

    //surfaces:
   
    painter.setOpacity(0.35);
    painter.drawPixmap(0, 0, getSurfaceTexture(tooth));
    painter.setOpacity(1);

    //the crown:

    painter.setOpacity(0.8);

    switch (tooth.prostho)
    {
    case ProsthoHint::none:
        break;
    case ProsthoHint::crown:
        painter.drawPixmap(coords.crownRect, *texturePack.crown);
        break;
    case ProsthoHint::crown_green:
        painter.drawPixmap(coords.crownRect, *texturePack.crown);
        painter.drawPixmap(coords.crownRect, textureFormat(*texturePack.crown, Qt::green, 0.3));
        break;
    default: break;
    }

    painter.setOpacity(1);

    return toothPx;

}

inline QPixmap getToothPixmap(const ToothPaintHint& tooth)
{
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& texturePack = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    QPixmap toothPx(coords.toothRect.width(), coords.toothCanvasHeight);
    toothPx.fill(Qt::transparent);
    QPainter painter(&toothPx);

    painter.translate(0, coords.impactedOffset);

    //drawing the splint from behind:
    painter.setOpacity(0.3);
    painter.drawPixmap(coords.frontSplintPaint, getSplintRect(tooth));
    painter.setOpacity(1);

    

    if (tooth.impacted) {

        painter.translate(0, -coords.impactedOffset);

        QPixmap toothOnly = getTooth(tooth);

        painter.drawPixmap(
            QRect(0, 0+40, coords.toothRect.width(), 360),
            toothOnly,
            QRect(0, 0, coords.toothRect.width(), 360)
        );

        painter.drawPixmap(
            QRect(0, 780-60, coords.toothRect.width(), 360),
            toothOnly,
            QRect(0, 500, coords.toothRect.width(), 360)
        );

        painter.translate(0, coords.impactedOffset);
    }
    else
    {
        painter.drawPixmap(0, 0, getTooth(tooth));
    }


    //drawing Bridge/splint from front:

    painter.setOpacity(0.8);

    switch (tooth.prostho)
    {
    case ProsthoHint::bridge:
        painter.drawPixmap(coords.crownRect, getBridgeTexture(tooth));
        break;
    case ProsthoHint::bridge_green:
    {
        QPixmap bridge = getBridgeTexture(tooth);
        painter.drawPixmap(coords.crownRect, bridge);
        painter.drawPixmap(coords.crownRect, textureFormat(bridge, GREEN, 0.3));
        break;
    }
    case ProsthoHint::splint:
        if (tooth.tooth != ToothTextureHint::normal || tooth.impacted)
        {
            painter.setOpacity(1);
            painter.drawPixmap(coords.crownRect, *texturePack.fiberOptic);
            painter.drawPixmap(coords.crownRect, (textureFormat(*texturePack.fiberOptic, QColor{ BLUE }, 0.3)));
        }
        painter.setOpacity(0.3);
        painter.drawPixmap(coords.lingualSplintPaint, getSplintRect(tooth));
        break;
    case ProsthoHint::splint_green:
        if (tooth.tooth != ToothTextureHint::normal)
        {
            painter.setOpacity(1);
            painter.drawPixmap(coords.crownRect, *texturePack.fiberOptic);
            painter.drawPixmap(coords.crownRect, (textureFormat(*texturePack.fiberOptic, QColor{ GREEN }, 0.3)));
        }
        painter.setOpacity(0.3);
        painter.drawPixmap(coords.lingualSplintPaint, getSplintRect(tooth));
        break;
    case ProsthoHint::denture:
        painter.setOpacity(0.5);
        painter.drawPixmap(coords.toothRect, getDenture(tooth));
        break;
    case ProsthoHint::denture_green:
        painter.drawPixmap(coords.toothRect, textureFormat(getDenture(tooth), GREEN, 0.4));
        painter.setOpacity(0.5);
        painter.drawPixmap(coords.toothRect, getDenture(tooth));        
        break;
    default: break;
    }

    painter.setOpacity(1);

    return toothPx;
}

inline void drawMobilityLabel(const ToothPaintHint& tooth, QPixmap& pixmap)
{
	if (!tooth.mobility) return;
    
    const char* mobilityLabels[] = {"", "I", "II", "III"};

	QPainter painter(&pixmap);

#ifdef Q_OS_WIN
    int pointSize = 28;
#else
    int pointSize = 35;
#endif

    QFont font;
    font.setPointSize(pointSize);
	font.setBold(1);

    int yPos = tooth.idx > 15 ? pixmap.height() - 123 : 50;

	painter.setPen(QColor{ 255, 146, 148 });
	painter.setFont(font);
    painter.drawText(QRect{ 0, yPos, pixmap.width(), 50}, Qt::AlignCenter, mobilityLabels[tooth.mobility]);

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

inline void drawQuestionMark(const ToothPaintHint& tooth, QPixmap& pixmap)
{
    if (tooth.tooth != ToothTextureHint::unknown) return;

    int yPos = tooth.idx > 15 ? 280 : pixmap.height() - 420;

    QPainter painter(&pixmap);
    
    QRect rect(0, yPos, pixmap.width(), 150);

    QPen pen(Qt::lightGray);
    pen.setWidth(20);
    painter.setPen(pen);

    painter.setRenderHint(QPainter::Antialiasing);

    QFont font{ "Arial", 80 };
    font.setBold(1);
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, "?");
}

inline void drawToothNumberLabel(const ToothPaintHint& tooth, QPixmap& pixmap)
{
	QPainter painter(&pixmap);

	int yPos = tooth.idx > 15 ? pixmap.height() - 50 : 0;

    QFont font;//{ "Arial", 28 };
    font.setPointSizeF(30);
	font.setBold(1);
    painter.setPen(Qt::gray);
	painter.setFont(font);

	painter.drawText(QRect{ 0,yPos,pixmap.width(), 50 }, Qt::AlignCenter, tooth.num.c_str());
}


void rotateByQuadrant(QPainter& painter, int textureWidth, int textureHeight, Dental::Quadrant quadrant)
{

    switch (quadrant)
    {
    case Dental::Quadrant::First:
        return;
    case Dental::Quadrant::Second:
        painter.setTransform(QTransform::fromScale(-1, 1));
        painter.translate(-textureWidth, 0);
        return;
    case Dental::Quadrant::Third:
        painter.rotate(180);
        painter.translate(-textureWidth, -textureHeight);
        break;
    case Dental::Quadrant::Fourth:
        painter.rotate(180);
        painter.setTransform(QTransform::fromScale(1, -1));
        painter.translate(0, -textureHeight);
        break;
    }

}

void rotateByQuadrantLingual(QPainter& painter, int textureWidth, int textureHeight, Dental::Quadrant q)
{
    switch (q)
    {
    case Dental::Quadrant::First: q = Dental::Quadrant::Third; break;
    case Dental::Quadrant::Second: q = Dental::Quadrant::Fourth; break;
    case Dental::Quadrant::Third: q = Dental::Quadrant::First; break;
    case Dental::Quadrant::Fourth: q = Dental::Quadrant::Second; break;
    }

    rotateByQuadrant(painter, textureWidth, textureHeight, q);
}

QPixmap ToothPainter::getBuccalOcclusal(const ToothPaintHint& tooth)
{
	constexpr int pixmapHeight = 746;

	auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);

	QPixmap pixmap(coords.toothRect.width(), pixmapHeight);
	pixmap.fill(Qt::transparent);

	drawMobilityLabel(tooth, pixmap);
	drawFractureLabel(tooth, pixmap);

    drawToothNumberLabel(tooth, pixmap);


    QPainter painter(&pixmap);

    rotateByQuadrant(painter, coords.toothRect.width(), pixmapHeight, ToothUtils::getQuadrant(tooth.idx));

    painter.drawPixmap(QRect(0, 0,
         coords.toothRect.width(), 640),
         getToothPixmap(tooth), coords.BuccalOcclusalCrop);

    painter.end();

    drawQuestionMark(tooth, pixmap);

    return pixmap;
}

QPixmap ToothPainter::getBuccalLingual(const ToothPaintHint& tooth)
{
    constexpr int pixmapHeight = 1106;

    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);

    QPixmap pixmap(coords.toothRect.width(), pixmapHeight);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    rotateByQuadrant(painter, coords.toothRect.width(), pixmapHeight, ToothUtils::getQuadrant(tooth.idx));

    painter.drawPixmap(0, 0, getToothPixmap(tooth));
    //painter.eraseRect(0, 123 + 360, coords.toothRect.width(), 140);


    return pixmap;
}


QPixmap ToothPainter::getOcclusal(const ToothPaintHint& tooth)
{
    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);
    auto& currentTexture = SpriteSheets::container().getTexturePack(tooth.idx, tooth.temp);

    if (tooth.tooth != ToothTextureHint::normal && tooth.tooth != ToothTextureHint::unknown) return QPixmap();

    QPixmap toothPx(coords.toothRect.width(), coords.toothRect.height());
    toothPx.fill(Qt::transparent);

    QPainter toothPainter(&toothPx);
/*
    if (tooth.calculus)
    {
        toothPainter.drawPixmap(coords.crownRect, *currentTexture.calculus);
    }
*/
    toothPainter.drawPixmap(0, 0, *currentTexture.tooth);

    toothPainter.setOpacity(0.35);
    toothPainter.drawPixmap(0, 0, getSurfaceTexture(tooth));

    toothPainter.setOpacity(0.35);
    if (tooth.prostho != ProsthoHint::none && tooth.prostho != ProsthoHint::splint && tooth.prostho != ProsthoHint::splint_green)
    {
        if (tooth.prostho == ProsthoHint::crown || tooth.prostho == ProsthoHint::bridge)
        {
            toothPainter.drawPixmap(coords.crownRect, *currentTexture.crown);
        }
        else
        {
            toothPainter.drawPixmap(coords.crownRect, *currentTexture.crown);
            toothPainter.drawPixmap(coords.crownRect, textureFormat(*currentTexture.crown, Qt::green, 0.3));
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

QPixmap ToothPainter::getLingualOcclusal(const ToothPaintHint& tooth)
{
    constexpr int pixmapHeight = 746;

    auto& coords = SpriteSheets::container().getCoordinates(tooth.idx, tooth.temp);

    QPixmap pixmap(coords.toothRect.width(), pixmapHeight);
    pixmap.fill(Qt::transparent);


    drawMobilityLabel(tooth, pixmap);
    drawFractureLabel(tooth, pixmap);


    drawToothNumberLabel(tooth, pixmap);

    QPainter painter(&pixmap);

    constexpr int toothYPosition = 123;

    rotateByQuadrantLingual(painter, coords.toothRect.width(), pixmapHeight, ToothUtils::getQuadrant(tooth.idx));

    painter.drawPixmap(
        QRect(0, toothYPosition, coords.toothRect.width(), 640),
        getToothPixmap(tooth), 
        coords.lingualOcclusalCrop
    );

    painter.end();

    drawQuestionMark(tooth, pixmap);

    return pixmap;
}
