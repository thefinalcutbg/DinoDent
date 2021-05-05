#pragma once
#include "ToothPainter2.h"

class CPToothPainter : public ToothPainter2
{
	virtual QPixmap returnPaintedTooth(const PaintHint& tooth) override;
public:
	
	QPixmap* paintTooth(const PaintHint& tooth) override;
};

