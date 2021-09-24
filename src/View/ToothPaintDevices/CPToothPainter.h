#pragma once
#include "ToothPainter.h"

class CPToothPainter : public ToothPainter
{
	virtual QPixmap returnPaintedTooth(const ToothPaintHint& tooth) override;
public:
	
	QPixmap* paintTooth(const ToothPaintHint& tooth) override;
};

