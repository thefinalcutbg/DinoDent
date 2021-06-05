#pragma once
#include <QPixmap>
#include <array>

struct ToothSprite
{
	QPixmap *tooth;
	QPixmap *root;
	QPixmap *post;
	QPixmap *endo;
	QPixmap *crown;
	QPixmap *lesion;
	QPixmap *paro;
	std::array<QPixmap*, 6> *surfaces;

};

