#pragma once
#include <QRect>
#include <QPoint>

enum class SpriteRectType{molar, premolar, frontal, tempFrontal, tempMolar};
enum surf {occ, med, dist, buc, ling, cerv};

struct SpritesheetCoords
{
    SpritesheetCoords(SpriteRectType type);
    QRect toothCrop; //the Rect of the tooth texture;

    QRect surfCrop[6]; //coordinates of the surfaces
    QRect endoCrop;     //coordinates for the endodontic system
    QRect perioCrop;
    QRect lesionCrop;
    QRect crownCrop;
    QRect rootCrop;
    QRect postCrop;
    QRect CPcrop;      //coordinates for the control panel

    QRect bridgeCrop;
    QRect bridgeHalfCrop;

    QRect implantCrop;
    QRect implantPaint;

    QPoint crownPos;    //position of the crown painting
    QPoint postPos;
    QPoint surfPos[6];  //positions for painting of the surfaces on the canavas


private:
    void molar();
    void premolar();
    void frontal();
};

