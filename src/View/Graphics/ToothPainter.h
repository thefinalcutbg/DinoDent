#pragma once

#include <QPixmap>
#include "PaintHint.h"

namespace ToothPainter
{
	QPixmap getBuccalOcclusal(const ToothPaintHint& toothHint);
	QPixmap getBuccalLingual(const ToothPaintHint& toothHint);
	QPixmap getSurfacePanel(const ToothPaintHint& toothHint, bool occlusal = true);
	QPixmap getLingualOcclusal(const ToothPaintHint& toothHint);
	
}
