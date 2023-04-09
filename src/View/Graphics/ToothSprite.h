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
	QPixmap* fiberOptic;
	QPixmap* lesion;
	QPixmap* perio;
	QPixmap* implant;
	QPixmap* calculus;
	QPixmap* lesionImplant;
	QPixmap* perioImplant;
	QPixmap* bridgeConnected;
	QPixmap* bridgeSeparated;
	QPixmap* falseTooth;
	QPixmap* denture;
	std::array<QPixmap*, 6> surfaces;

	inline static QPoint crownPaint{ 0, 210 };

	~TexturePack()
	{
		//don't delete implant textures, they are common!
		delete tooth;
		delete root;
		delete post;
		delete endo;
		delete fiberOptic;
		delete crown;
		delete calculus;
		delete lesion;
		delete perio;
		delete bridgeConnected;
		delete bridgeSeparated;
		delete falseTooth;
		
		for (int i = 0; i < 6; i++)
		{
		 delete surfaces[i];
		}
	}

};

