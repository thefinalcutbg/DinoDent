#include "SpriteRect.h"

SpritesheetCoords::SpritesheetCoords(SpriteRectType type)
{

	implantCrop.setCoords(0, 0, 120, 250);
	implantLesionCrop.setCoords(120, 0, 240, 250);
	implantPerioCrop.setCoords(240, 0, 360, 250);

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
	toothCrop.setCoords(0, 0, 180, 500);

	surfCrop[0].setCoords(180, 360, 360, 500);     //occlusal surface
	surfCrop[1].setCoords(450, 250, 540, 500);    //medial surface
	surfCrop[2].setCoords(360, 250, 460, 500);    //distal stuface
	surfCrop[3].setCoords(540, 250, 720, 430);     //buccal surface
	surfCrop[4].setCoords(540, 430, 720, 500);      //lingual surface
	surfCrop[5].setCoords(360, 210, 540, 250);       //cervical surface

	postCrop.setCoords(720, 250, 900, 520);	
	rootCrop.setCoords(540, 0, 720, 250);
	perioCrop.setCoords(720, 0, 900, 240);
	endoCrop.setCoords(180, 0, 360, 210);       
	lesionCrop.setCoords(360, 0, 540, 80);
	crownCrop.setCoords(180, 210, 360, 360);
	occlusalErase.setCoords(0, 360, 180, 140);
	implantPaint.setCoords(30, 0, 150, 240);

	surfPos[occ] = QPoint(0, 360);
	surfPos[med] = QPoint(90, 250);
	surfPos[dist] = QPoint(0, 250);
	surfPos[buc] = QPoint(0, 250);
	surfPos[ling] = QPoint(0, 430);
	surfPos[cerv] = QPoint(0, 210);

	postPos = QPoint(0, 100);
	crownPos = QPoint(0, 210);

	CPcrop.setCoords(25, 360, 165, 500);
}

void SpritesheetCoords::premolar()
{
	toothCrop.setCoords(0, 0, 120, 500);

	surfCrop[occ].setCoords(120, 360, 240, 500);
	surfCrop[med].setCoords(300, 250, 360, 500);
	surfCrop[dist].setCoords(240, 250, 300, 500);
	surfCrop[buc].setCoords(360, 250, 480, 430);
	surfCrop[ling].setCoords(360, 430, 480, 500);
	surfCrop[cerv].setCoords(240, 210, 360, 250);

	rootCrop.setCoords(360, 0, 480, 250);
	perioCrop.setCoords(480, 0, 600, 240);
	endoCrop.setCoords(120, 0, 240, 210);
	postCrop.setCoords(480, 250, 600, 430);
	lesionCrop.setCoords(240, 0, 360, 80);
	crownCrop.setCoords(120, 210, 240, 360);
	occlusalErase.setCoords(0, 360, 120, 140);
	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 360);
	surfPos[med] = QPoint(60, 250);
	surfPos[dist] = QPoint(0, 250);
	surfPos[buc] = QPoint(0, 250);
	surfPos[ling] = QPoint(0, 430);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);
	postPos = QPoint(0, 100);
	CPcrop.setCoords(12, 360, 106, 476);


}

void SpritesheetCoords::frontal()
{
	toothCrop.setCoords(0, 0, 120, 500);

	surfCrop[occ].setCoords(120, 250, 240, 430);
	surfCrop[med].setCoords(300, 250, 360, 500);
	surfCrop[dist].setCoords(240, 250, 300, 500);
	surfCrop[buc].setCoords(360, 250, 480, 400);
	surfCrop[ling].setCoords(360, 400, 480, 500);
	surfCrop[cerv].setCoords(240, 210, 360, 250);

	rootCrop.setCoords(360, 0, 480, 250);
	perioCrop.setCoords(480, 0, 600, 240);
	endoCrop.setCoords(120, 0, 240, 210);
	postCrop.setCoords(480, 250, 600, 430);
	lesionCrop.setCoords(240, 0, 360, 80);
	crownCrop.setCoords(600, 210, 720, 360);
	occlusalErase.setCoords(0, 360, 120, 140);

	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 250);
	surfPos[med] = QPoint(60, 250);
	surfPos[dist] = QPoint(0, 250);
	surfPos[buc] = QPoint(0, 250);
	surfPos[ling] = QPoint(0, 400);
	surfPos[cerv] = QPoint(0, 210);

	crownPos = QPoint(0, 210);
	postPos = QPoint(0, 100);

	CPcrop.setCoords(-10, 210, 120, 355);

}

void SpritesheetCoords::tempfrontal()
{
	toothCrop.setCoords(0, 0, 120, 500);

	surfCrop[occ].setCoords(120, 255, 240, 430);
	surfCrop[med].setCoords(300, 225, 360, 430);
	surfCrop[dist].setCoords(240, 225, 300, 500);
	surfCrop[buc].setCoords(360, 225, 480, 400);
	surfCrop[ling].setCoords(360, 400, 480, 450);
	surfCrop[cerv].setCoords(120, 225, 240, 255);

	rootCrop.setCoords(480, 0, 600, 255);
	perioCrop.setCoords(600, 0, 720, 255);
	endoCrop.setCoords(120, 0, 240, 225);
	lesionCrop.setCoords(240, 0, 360, 160);
	crownCrop.setCoords(600, 365, 720, 500);
	occlusalErase.setCoords(0, 360, 120, 140);

	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 255);
	surfPos[med] = QPoint(60, 225);
	surfPos[dist] = QPoint(0, 225);
	surfPos[buc] = QPoint(0, 225);
	surfPos[ling] = QPoint(0, 400);
	surfPos[cerv] = QPoint(0, 225);

	crownPos = QPoint(0, 225);
	//postPos = QPoint(0, 0);

	CPcrop.setCoords(20, 225, 100, 310);


}

void SpritesheetCoords::tempmolar()
{
	toothCrop.setCoords(0, 0, 120, 500);

	surfCrop[occ].setCoords(120, 360, 240, 500);
	surfCrop[med].setCoords(300, 240, 360, 500);
	surfCrop[dist].setCoords(240, 240, 300, 500);
	surfCrop[buc].setCoords(360, 240, 480, 415);
	surfCrop[ling].setCoords(360, 415, 480, 500);
	surfCrop[cerv].setCoords(240, 225, 360, 240);

	rootCrop.setCoords(360, 0, 480, 240);
	perioCrop.setCoords(480, 0, 600, 240);
	endoCrop.setCoords(120, 0, 240, 225);
	postCrop.setCoords(0,0,0, 0);
	lesionCrop.setCoords(240, 0, 360, 140);
	crownCrop.setCoords(480, 365, 600, 500);
	occlusalErase.setCoords(0, 360, 120, 140);

	implantPaint = implantCrop;

	surfPos[occ] = QPoint(0, 360);
	surfPos[med] = QPoint(60, 240);
	surfPos[dist] = QPoint(0, 240);
	surfPos[buc] = QPoint(0, 240);
	surfPos[ling] = QPoint(0, 415);
	surfPos[cerv] = QPoint(0, 225);

	crownPos = QPoint(0, 225);
	postPos = QPoint(0, 100);
	CPcrop.setCoords(5, 370, 120, 470);

}

