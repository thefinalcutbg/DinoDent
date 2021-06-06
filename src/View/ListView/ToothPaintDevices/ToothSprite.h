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
	QPixmap* paro;
	std::array<QPixmap*, 6> surfaces;

	~ToothSprite()
	{
		delete tooth;
		delete root;
		delete post;
		delete endo;
		delete crown;
		delete lesion;
		delete paro;
		for (int i = 0; i < 6; i++)
		{
		 delete surfaces[i];
		}
	}

};

