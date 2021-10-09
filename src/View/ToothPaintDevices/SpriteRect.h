#pragma once
#include <QRect>
#include <QPoint>

enum class SpriteRectType{molar, premolar, frontal, tempFrontal, tempMolar};
enum surf {occ, med, dist, buc, ling, cerv};

struct SpritesheetCoords
{
    SpritesheetCoords(SpriteRectType type);
    QRect toothCrop; //the Rect of the tooth texture;

    QRect occlusalErase;

    QRect buccalImpactCrop;
    QRect lingualImpactCrop;

    QRect SurfacePanelCrop;      //coordinates for the control panel
    QRect BuccalOcclusalCrop; 



    QRect implantLesionCrop;
    QRect implantPerioCrop;

    QRect implantCrop;
    QRect implantPaint;

    QPoint crownPos;    //position of the crown painting
    QPoint surfPos[6];  //positions for painting of the surfaces on the canavas


private:
    void molar();
    void premolar();
    void frontal();
    void tempfrontal();
    void tempmolar();
};

