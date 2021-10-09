#include "SpriteRect.h"

SpritesheetCoords::SpritesheetCoords(SpriteRectType type)
{

	implantCrop.setCoords(0, 0, 120, 860);
	implantLesionCrop.setCoords(120, 0, 240, 860);
	implantPerioCrop.setCoords(240, 0, 360, 860);

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
	toothCrop.setCoords(0, 0, 180, 860);

	occlusalErase.setCoords(0, 360, 180, 140);
	

	buccalImpactCrop.setCoords(0, 0, 180, 360);
	lingualImpactCrop.setCoords(0, 500, 180, 860);
	
	implantPaint.setCoords(30, 0, 150, 860);

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(90, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);

	SurfacePanelCrop.setCoords(25, 360, 165, 500);
	BuccalOcclusalCrop.setCoords(0, 0, 180, 500);

}

void SpritesheetCoords::premolar()
{
	toothCrop.setCoords(0, 0, 120, 860);


	occlusalErase.setCoords(0, 360, 180, 140);
	implantPaint = implantCrop;

	buccalImpactCrop.setCoords(0, 0, 120, 360);
	lingualImpactCrop.setCoords(0, 500, 120, 860);

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(60, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);

	SurfacePanelCrop.setCoords(12, 360, 106, 476);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 500);


}

void SpritesheetCoords::frontal()
{
	toothCrop.setCoords(0, 0, 120, 860);


	occlusalErase.setCoords(0, 360, 120, 140);
	implantPaint = implantCrop;

	buccalImpactCrop.setCoords(0, 0, 120, 360);
	lingualImpactCrop.setCoords(0, 500, 120, 860);

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(60, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);

	SurfacePanelCrop.setCoords(-10, 210, 120, 355);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 500);
}

void SpritesheetCoords::tempfrontal()
{
	toothCrop.setCoords(0, 0, 120, 500);

	occlusalErase.setCoords(0, 360, 120, 140);

	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(60, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 350);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);

	SurfacePanelCrop.setCoords(20, 205, 100, 290);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 500);


}

void SpritesheetCoords::tempmolar()
{
	toothCrop.setCoords(0, 0, 120, 500);

	occlusalErase.setCoords(0, 360, 120, 140);

	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 210);
	surfPos[med] = QPoint(60, 210);
	surfPos[dist] = QPoint(0, 210);
	surfPos[buc] = QPoint(0, 210);
	surfPos[ling] = QPoint(0, 210);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);

	SurfacePanelCrop.setCoords(5, 370, 120, 470);
	BuccalOcclusalCrop.setCoords(0, 0, 120, 500);

}

