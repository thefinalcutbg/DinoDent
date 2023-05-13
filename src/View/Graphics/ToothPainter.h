#pragma once

#include <QPixmap>
#include "PaintHint.h"

namespace ToothPainter
{
	QPixmap getBuccalOcclusal(const ToothPaintHint& toothHint);
	QPixmap getBuccalLingual(const ToothPaintHint& toothHint);
	QPixmap getOcclusal(const ToothPaintHint& toothHint);
	QPixmap getLingualOcclusal(const ToothPaintHint& toothHint);
	
}
