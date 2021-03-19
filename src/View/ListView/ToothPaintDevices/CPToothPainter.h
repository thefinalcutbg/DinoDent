#pragma once
#include "ToothPainter.h"

class CPToothPainter : public ToothPainter
{
	virtual QPixmap paintToothStatus();
public:
	
	virtual QPixmap* getPixmap(const Tooth& tooth);
};

