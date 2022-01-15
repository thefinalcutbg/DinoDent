#pragma once
#include <QRect>
#include <QPoint>

enum class SpriteRectType{molar, premolar, frontal, tempFrontal, tempMolar};
enum surf {occ, med, dist, buc, ling, cerv};

struct SpritesheetCoords
{
    SpritesheetCoords(SpriteRectType type);
    QRect toothRect; //the rect of the whole tooth texture;
    QRect crownRect; //the rect which contains the crown parts of the tooth

    QRect frontSplintPaint;
    QRect lingualSplintPaint;

    QRect buccalImpactCrop;
    QRect lingualImpactCrop;

    QRect SurfacePanelCrop;      //coordinates for the control panel
    QRect BuccalOcclusalCrop; 



    QRect implantLesionCrop;
    QRect implantPerioCrop;

    QRect implantCrop;

    QPoint implantPos;

    QPoint surfPos[6];  //positions for painting of the surfaces on the canavas


private:
    void molar();
    void premolar();
    void frontal();
    void tempfrontal();
    void tempmolar();
};

