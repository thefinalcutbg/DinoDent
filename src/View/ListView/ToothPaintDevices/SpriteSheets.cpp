#include "SpriteSheets.h"

SpriteSheets SpriteSheets::instance;

SpriteSheets::SpriteSheets()
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
        QString indx = QString::number(y);

        maxTexture[i] = new QPixmap("toothimage/1" + indx + ".png");/*qApp->applicationDirPath()+*/
        mandTexture[i] = new QPixmap("toothimage/3" + indx + ".png");

        permaTexture[i] = maxTexture[i];        //arranging the textures in array of pointers;
        permaTexture[15 - i] = maxTexture[i];
        permaTexture[16 + i] = mandTexture[i];
        permaTexture[31 - i] = mandTexture[i];
    }

    //loading deciduous teeth:
    for (int i = 0, y = 5; i < 5; i++, y--)
    {
        QString indx = QString::number(y);

        maxDTexture[i] = new QPixmap("toothimage/5" + indx + ".png");
        mandDTexture[i] = new QPixmap("toothimage/7" + indx + ".png");

        deciTexture[i] = maxDTexture[i];          //1st quadrant
        deciTexture[9 - i] = maxDTexture[i];      //2nd quadrant
        deciTexture[10 + i] = mandDTexture[i];    //3rd quadrant
        deciTexture[19 - i] = mandDTexture[i];    //4th quadrant
    }

    commonTexture = new QPixmap("toothimage/common.png");

    bridgeU = new QPixmap("toothimage/bridgeUP.png");

    bridgeL = new QPixmap("toothimage/bridgeLOW.png");

}

SpriteSheets::~SpriteSheets()
{
    for (int i = 0; i < 8; i++)
    {
        delete maxTexture[i];
        delete mandTexture[i];
        if (i < 5)
        {
            delete maxDTexture[i];
            delete mandDTexture[i];
        }
    }

    delete commonTexture;
}

SpriteSheets& SpriteSheets::container()
{
    return instance;
}

void SpriteSheets::LoadSpriteSheets()
{ 
    instance.loadTextures();
}

QPixmap* SpriteSheets::getTexture(int toothIndex, bool temporary)
{
    if (temporary)
        return deciTexture[permaToTemp_map[toothIndex]];
    else
        return permaTexture[toothIndex];
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
