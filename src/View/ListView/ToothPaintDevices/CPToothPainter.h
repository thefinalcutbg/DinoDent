#pragma once
#include "ToothPainter.h"

class CPToothPainter : public ToothPainter
{
	virtual QPixmap returnPaintedTooth(const PaintHint& tooth) override;
public:
	
	QPixmap* paintTooth(const PaintHint& tooth) override;
};

