#pragma once

#include <QPixmap>
#include "PaintHint.h"

namespace ToothPainter
{
	QPixmap getBuccalOcclusal(const ToothPaintHint& toothHint);
	QPixmap getBuccalLingual(const ToothPaintHint& toothHint);
	QPixmap getOcclusal(const ToothPaintHint& toothHint);
	QPixmap getLingualOcclusal(const ToothPaintHint& toothHint);

	QPixmap getDnsBuccal(const ToothPaintHint& toothHint);
	QPixmap getDnsBuccalLingual(const ToothPaintHint& toothHint);
	QPixmap getDnsLingual(const ToothPaintHint& toothHint);
	
}
