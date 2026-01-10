#include "SpriteRect.h"

SpritesheetCoords::SpritesheetCoords(SpriteRectType type)
{

	implantCrop.setCoords(0, 0, 120, 860);
	implantLesionCrop.setCoords(120, 0, 240, 860);
	implantPerioCrop.setCoords(240, 0, 360, 860);

	implantPos = QPoint{0, 0};

	toothRect.setCoords(0, 0, 120, 860);
	crownRect.setCoords(0, 210, 120, 640);

	frontSplintPaint.setCoords(0, 210, 120, 340);
	lingualSplintPaint.setCoords(0, 500, 120, 650);

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(60, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);

	switch (type)
	{
		case SpriteRectType::molar:
			molar();
			break;
		case SpriteRectType::premolar:
			premolar();
			break;
		case SpriteRectType::frontal:
			frontal();
			break;
		case SpriteRectType::tempFrontal:
			tempfrontal();
			break;
		case SpriteRectType::tempMolar:
			tempmolar();
			break;
	}
}

void SpritesheetCoords::molar()
{
	toothRect.setCoords(0, 0, 180, 860);
	crownRect.setCoords(0, 210, 180, 640);

	frontSplintPaint.setCoords(0, 210, 180, 340);
	lingualSplintPaint.setCoords(0, 500, 180, 650);

	buccalCrop.setCoords(0, 0, 180, 360);
	lingualCrop.setCoords(0, 500, 180, 860);

	implantPos = QPoint{0, 0 };

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(90, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);


	SurfacePanelCrop.setCoords(25, 360, 165, 500);
	BuccalOcclusalCrop.setCoords(0, 0, 180, 640);
	lingualOcclusalCrop.setCoords(0, 490, 180, 1140);

	implantCrop.setCoords(0, 0, 180, 860);
	implantLesionCrop.setCoords(180, 0, 360, 860);
	implantPerioCrop.setCoords(360, 0, 540, 860);

}

void SpritesheetCoords::premolar()
{

	buccalCrop.setCoords(0, 0, 120, 360);
	lingualCrop.setCoords(0, 500, 120, 860);

	SurfacePanelCrop.setCoords(12, 360, 106, 476);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 640);
	lingualOcclusalCrop.setCoords(0, 490, 120, 1140);
}

void SpritesheetCoords::frontal()
{
	buccalCrop.setCoords(0, 0, 120, 360);
	lingualCrop.setCoords(0, 500, 120, 860);

	SurfacePanelCrop.setCoords(-10, 210, 120, 355);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 640);
	lingualOcclusalCrop.setCoords(0, 490, 120, 1140);
}

void SpritesheetCoords::tempfrontal()
{
	SurfacePanelCrop.setCoords(20, 205, 100, 290);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 640);
	lingualOcclusalCrop.setCoords(0, 490, 120, 1140);
}

void SpritesheetCoords::tempmolar()
{

	SurfacePanelCrop.setCoords(5, 365, 120, 465);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 640);
	lingualOcclusalCrop.setCoords(0, 490, 120, 1140);

}

