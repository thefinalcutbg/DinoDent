#pragma once
#include <QPixmap>
#include <array>

struct TexturePack
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

	~TexturePack()
	{
		//don't delete implant textures, they are common!
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

