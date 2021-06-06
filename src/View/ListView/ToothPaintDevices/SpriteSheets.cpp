#include "SpriteSheets.h"

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
   

}

void SpriteSheets::loadTextures()
{
    //loading the textures of the permanent teeth
    for (int i = 0, y = 8; i < 8; i++, y--)
    {
        SpritesheetCoords* coord(nullptr);
        if (i < 3) coord = &molarRect;
        else if (i < 5) coord = &premolarRect;
        else coord = &frontalRect;

        QString indx = QString::number(y);
        {
        QPixmap maxTx("toothimage/1" + indx + ".png");/*qApp->applicationDirPath()+*/

        maxPermanentSprites[i].tooth = textureCut(maxTx, coord->toothCrop);
        maxPermanentSprites[i].root = textureCut(maxTx, coord->rootCrop);
        maxPermanentSprites[i].endo = textureCut(maxTx, coord->endoCrop);
        maxPermanentSprites[i].crown = textureCut(maxTx, coord->crownCrop);
        maxPermanentSprites[i].lesion = textureCut(maxTx, coord->lesionCrop);
        maxPermanentSprites[i].post = textureCut(maxTx, coord->postCrop);
        maxPermanentSprites[i].paro = textureCut(maxTx, coord->perioCrop);
        for (int j = 0; j < 6; j++)
            maxPermanentSprites[i].surfaces[j] = textureCut(maxTx, coord->surfCrop[j]);
        }

        QPixmap mandTx("toothimage/3" + indx + ".png");

        mandPermanentSprites[i].tooth = textureCut(mandTx, coord->toothCrop);
        mandPermanentSprites[i].root = textureCut(mandTx, coord->rootCrop);
        mandPermanentSprites[i].endo = textureCut(mandTx, coord->endoCrop);
        mandPermanentSprites[i].crown = textureCut(mandTx, coord->crownCrop);
        mandPermanentSprites[i].lesion = textureCut(mandTx, coord->lesionCrop);
        mandPermanentSprites[i].post = textureCut(mandTx, coord->postCrop);
        mandPermanentSprites[i].paro = textureCut(mandTx, coord->perioCrop);
        for (int j = 0; j < 6; j++)
            mandPermanentSprites[i].surfaces[j] = textureCut(mandTx, coord->surfCrop[j]);

   

        permaTexture[i] = &maxPermanentSprites[i];        //arranging the textures in array of pointers;
        permaTexture[15 - i] = &maxPermanentSprites[i];
        permaTexture[16 + i] = &mandPermanentSprites[i];
        permaTexture[31 - i] = &mandPermanentSprites[i];
    }

    //loading deciduous teeth:
    for (int i = 0, y = 5; i < 5; i++, y--)
    {
        SpritesheetCoords* coord(nullptr);
        if (i < 2) coord = &tempMolarsRect;
        else coord = &tempFrontalRect;


        QString indx = QString::number(y);

        QPixmap maxTx("toothimage/5" + indx + ".png");

        maxTemporarySprites[i].tooth = textureCut(maxTx, coord->toothCrop);
        maxTemporarySprites[i].root = textureCut(maxTx, coord->rootCrop);
        maxTemporarySprites[i].endo = textureCut(maxTx, coord->endoCrop);
        maxTemporarySprites[i].crown = textureCut(maxTx, coord->crownCrop);
        maxTemporarySprites[i].lesion = textureCut(maxTx, coord->lesionCrop);
        maxTemporarySprites[i].post = textureCut(maxTx, coord->postCrop);
        maxTemporarySprites[i].paro = textureCut(maxTx, coord->perioCrop);
        for (int j = 0; j < 6; j++)
            maxTemporarySprites[i].surfaces[j] = textureCut(maxTx, coord->surfCrop[j]);



        QPixmap mandTx("toothimage/7" + indx + ".png");

        mandTemporarySprites[i].tooth = textureCut(mandTx, coord->toothCrop);
        mandTemporarySprites[i].root = textureCut(mandTx, coord->rootCrop);
        mandTemporarySprites[i].endo = textureCut(mandTx, coord->endoCrop);
        mandTemporarySprites[i].crown = textureCut(mandTx, coord->crownCrop);
        mandTemporarySprites[i].lesion = textureCut(mandTx, coord->lesionCrop);
        mandTemporarySprites[i].post = textureCut(mandTx, coord->postCrop);
        mandTemporarySprites[i].paro = textureCut(mandTx, coord->perioCrop);
        for (int j = 0; j < 6; j++)
            mandTemporarySprites[i].surfaces[j] = textureCut(mandTx, coord->surfCrop[j]);




        deciTexture[i] = &maxTemporarySprites[i];          //1st quadrant
        deciTexture[9 - i] = &maxTemporarySprites[i];      //2nd quadrant
        deciTexture[10 + i] = &mandTemporarySprites[i];    //3rd quadrant
        deciTexture[19 - i] = &mandTemporarySprites[i];    //4th quadrant
    }

    commonTexture = new QPixmap("toothimage/common.png");

    bridgeU = new QPixmap("toothimage/bridgeUP.png");

    bridgeL = new QPixmap("toothimage/bridgeLOW.png");

}

QPixmap* SpriteSheets::textureCut(const QPixmap& spriteSheet, QRect rect)
{
    QPixmap* px = new QPixmap(spriteSheet.copy(rect));
    return px;
}

SpriteSheets::~SpriteSheets()
{

    delete commonTexture;
    delete bridgeL;
    delete bridgeU;
}

SpriteSheets& SpriteSheets::container()
{
    return instance;
}

void SpriteSheets::loadSpriteSheets()
{ 
    instance.loadTextures();
}

const ToothSprite& SpriteSheets::getTexture(int toothIndex, bool temporary)
{
    if (temporary)
        return *deciTexture[permaToTemp_map[toothIndex]];
    else
        return *permaTexture[toothIndex];
}


QPixmap* SpriteSheets::getCommonTexture()
{
    return commonTexture;
}

QPixmap* SpriteSheets::getUpperBridge()
{
    return bridgeU;
}

QPixmap* SpriteSheets::getLowerBridge()
{
    return bridgeL;
}

QPixmap SpriteSheets::getRawBridge(int tooth_idx)
{
    QPixmap* currentPixmap;

    tooth_idx < 16 ? currentPixmap = bridgeU : currentPixmap = bridgeL;

    if (tooth_idx >= 16) tooth_idx = tooth_idx % 16;

    if (tooth_idx > 7) tooth_idx = 15 - tooth_idx;
 
    int height = 300;
    int width_begin = 0;

    for (int i = 0; i < tooth_idx; i++)
    {
        i < 3 ? width_begin = width_begin + 180 : width_begin = width_begin + 120;
    }

    int width_end = tooth_idx < 3 ? width_begin + 180 : width_begin + 120;

    return QPixmap(currentPixmap->copy(width_begin, 0, width_end, 300));


}
