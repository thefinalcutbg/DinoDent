#include "SpriteSheets.h"
#include <QPainter>
#include <Model/Tooth/ToothUtils.h>
#include <QFile>

QPixmap* textureCut(const QPixmap& spriteSheet, QRect rect)
{
    QPixmap* px = new QPixmap(spriteSheet.copy(rect));
    return px;
}

QPixmap* textureCut(const QPixmap& spriteSheet, QRect rect, QRect erase)
{
    QPixmap* px = new QPixmap(spriteSheet.copy(rect));
    QPainter painter(px);
    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Clear);
    painter.fillRect(erase, Qt::transparent);
    return px;
}



SpriteSheets SpriteSheets::instance;

SpriteSheets::SpriteSheets()
    :
    molarRect(SpriteRectType::molar),
    premolarRect(SpriteRectType::premolar),
    frontalRect(SpriteRectType::frontal),
    tempFrontalRect(SpriteRectType::tempFrontal),
    tempMolarsRect(SpriteRectType::tempMolar)
{

    int deciNum = 0;
    for (int i = 0; i < 32; i++)
    {
        if (i < 3) continue;
        else if (i > 12 && i < 19) continue;
        else if (i > 28) continue;
        permaToTemp_map[i] = deciNum;
        deciNum++;
    }


    for (int i = 0; i < 32; i++)
    {
        switch(ToothUtils::getToothType(i))
        {
        case ToothType::Molar:
            permanentCoords[i] = &molarRect;
            tempoCoords[i] = nullptr;
            break;
        case ToothType::Premolar:
            permanentCoords[i] = &premolarRect;
            tempoCoords[i] = &tempMolarsRect;
            break;
        case ToothType::Frontal:
            permanentCoords[i] = &frontalRect;
            tempoCoords[i] = &tempFrontalRect;
            break;
        }
    }
   

}

void SpriteSheets::loadTextures()
{

    QPixmap commonTexture("toothimage/common.png");
    implant = textureCut(commonTexture, molarRect.implantCrop);
    lesionImplant = textureCut(commonTexture, molarRect.implantLesionCrop);
    perioImplant = textureCut(commonTexture, molarRect.implantPerioCrop);



    //loading bridges;

    QPixmap bridgeU("toothimage/bridgeUP.png");

    QPixmap bridgeLowTest("toothimage/bridgeLOW2.png");

    int xPos = 0;
    int width = 180;

    for (int i = 0; i < 8; i++)
    {
        if (ToothUtils::getToothType(i) != ToothType::Molar) //a.k.a. i > 2 :D
            width = 120;

        rawBridgesU[i] = new QPixmap(bridgeU.copy(xPos, 0, width, 300));
        rawBridgesL[i] = new QPixmap(bridgeLowTest.copy(xPos, 0, width, 300));

        xPos = xPos + width;

    }




    //loading the textures of the permanent teeth
    for (int i = 0, y = 8; i < 8; i++, y--)
    {
        auto& coord = getCoordinates(i, false);

        QString indx = QString::number(y);
        {
        QPixmap maxTx("toothimage/1" + indx + ".png");/*qApp->applicationDirPath()+*/

        maxPermanentSprites[i].tooth = textureCut(maxTx, coord.toothCrop);
        maxPermanentSprites[i].root = textureCut(maxTx, coord.rootCrop, coord.rootAndPerioErase);
        maxPermanentSprites[i].endo = textureCut(maxTx, coord.endoCrop, coord.endoErase);
        maxPermanentSprites[i].crown = textureCut(maxTx, coord.crownCrop, coord.crownErase);
        maxPermanentSprites[i].lesion = textureCut(maxTx, coord.lesionCrop, coord.lesionErase);
        maxPermanentSprites[i].post = textureCut(maxTx, coord.postCrop);
        maxPermanentSprites[i].perio = textureCut(maxTx, coord.perioCrop, coord.rootAndPerioErase);
        maxPermanentSprites[i].implant = implant;
        maxPermanentSprites[i].lesionImplant = lesionImplant;
        maxPermanentSprites[i].perioImplant = perioImplant;
        for (int j = 0; j < 6; j++)
            maxPermanentSprites[i].surfaces[j] = textureCut(maxTx, coord.surfCrop[j]);
        }
        maxPermanentSprites[i].rawBridge = rawBridgesU[i];

        QPixmap mandTx("toothimage/3" + indx + ".png");

        mandPermanentSprites[i].tooth = textureCut(mandTx, coord.toothCrop);
        mandPermanentSprites[i].root = textureCut(mandTx, coord.rootCrop, coord.rootAndPerioErase);
        mandPermanentSprites[i].endo = textureCut(mandTx, coord.endoCrop, coord.endoErase);
        mandPermanentSprites[i].crown = textureCut(mandTx, coord.crownCrop, coord.crownErase);
        mandPermanentSprites[i].lesion = textureCut(mandTx, coord.lesionCrop, coord.lesionErase);
        mandPermanentSprites[i].post = textureCut(mandTx, coord.postCrop);
        mandPermanentSprites[i].perio = textureCut(mandTx, coord.perioCrop, coord.rootAndPerioErase);
        mandPermanentSprites[i].implant = implant;
        mandPermanentSprites[i].lesionImplant = lesionImplant;
        mandPermanentSprites[i].perioImplant = perioImplant;
        for (int j = 0; j < 6; j++)
            mandPermanentSprites[i].surfaces[j] = textureCut(mandTx, coord.surfCrop[j]);
        mandPermanentSprites[i].rawBridge = rawBridgesL[i];
   

        permaTexture[i] = &maxPermanentSprites[i];        //arranging the textures in array of pointers;
        permaTexture[15 - i] = &maxPermanentSprites[i];
        permaTexture[16 + i] = &mandPermanentSprites[i];
        permaTexture[31 - i] = &mandPermanentSprites[i];
    }

    //loading deciduous teeth:
    for (int i = 0, y = 5; i < 5; i++, y--)
    {
        auto& coord = getCoordinates(i + 3, true);

        QString indx = QString::number(y);

        QPixmap maxTx("toothimage/5" + indx + ".png");

        maxTemporarySprites[i].tooth = textureCut(maxTx, coord.toothCrop);
        maxTemporarySprites[i].root = textureCut(maxTx, coord.rootCrop);
        maxTemporarySprites[i].endo = textureCut(maxTx, coord.endoCrop);
        maxTemporarySprites[i].crown = textureCut(maxTx, coord.crownCrop);
        maxTemporarySprites[i].lesion = textureCut(maxTx, coord.lesionCrop);
        maxTemporarySprites[i].post = textureCut(maxTx, coord.postCrop);
        maxTemporarySprites[i].perio = textureCut(maxTx, coord.perioCrop);
        for (int j = 0; j < 6; j++)
            maxTemporarySprites[i].surfaces[j] = textureCut(maxTx, coord.surfCrop[j]);

        maxTemporarySprites[i].rawBridge = rawBridgesU[i + 3];


        QPixmap mandTx("toothimage/7" + indx + ".png");

        mandTemporarySprites[i].tooth = textureCut(mandTx, coord.toothCrop);
        mandTemporarySprites[i].root = textureCut(mandTx, coord.rootCrop);
        mandTemporarySprites[i].endo = textureCut(mandTx, coord.endoCrop);
        mandTemporarySprites[i].crown = textureCut(mandTx, coord.crownCrop);
        mandTemporarySprites[i].lesion = textureCut(mandTx, coord.lesionCrop);
        mandTemporarySprites[i].post = textureCut(mandTx, coord.postCrop);
        mandTemporarySprites[i].perio = textureCut(mandTx, coord.perioCrop);
        for (int j = 0; j < 6; j++)
            mandTemporarySprites[i].surfaces[j] = textureCut(mandTx, coord.surfCrop[j]);

        mandTemporarySprites[i].rawBridge = rawBridgesL[i + 3];

        deciTexture[i] = &maxTemporarySprites[i];          //1st quadrant
        deciTexture[9 - i] = &maxTemporarySprites[i];      //2nd quadrant
        deciTexture[10 + i] = &mandTemporarySprites[i];    //3rd quadrant
        deciTexture[19 - i] = &mandTemporarySprites[i];    //4th quadrant
    }


 

}



SpriteSheets::~SpriteSheets()
{
    delete implant;
    delete lesionImplant;
    delete perioImplant;

    for (int i = 0; i < 8; i++)
    {
        delete rawBridgesU[i];
        delete rawBridgesL[i];
    }
}

SpriteSheets& SpriteSheets::container()
{
    return instance;
}

const TexturePack& SpriteSheets::getTexturePack(int toothIndex, bool temporary)
{
    if (temporary)
        return *deciTexture[permaToTemp_map[toothIndex]];
    else
        return *permaTexture[toothIndex];
}

const SpritesheetCoords& SpriteSheets::getCoordinates(int toothIndex, bool temporary)
{
    return temporary ? *tempoCoords[toothIndex] : *permanentCoords[toothIndex];
}
