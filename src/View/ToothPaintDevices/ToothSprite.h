#pragma once
#include <QPixmap>
#include <array>

struct ToothSprite
{
	QPixmap* tooth;
	QPixmap* root;
	QPixmap* post;
	QPixmap* endo;
	QPixmap* crown;
	QPixmap* lesion;
	QPixmap* perio;
	QPixmap* implant;
	QPixmap* lesionImplant;
	QPixmap* perioImplant;
	std::array<QPixmap*, 6> surfaces;

	~ToothSprite()
	{
		delete tooth;
		delete root;
		delete post;
		delete endo;
		delete crown;
		delete lesion;
		delete perio;
		for (int i = 0; i < 6; i++)
		{
		 delete surfaces[i];
		}
	}

};

