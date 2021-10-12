#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "View/ToothPaintDevices/PaintHint.h"

class ToothContainer;

namespace ToothHintCreator
{

	ToothPaintHint getToothHint(const Tooth& tooth);
	std::array<ToothPaintHint, 32> getTeethHint(const ToothContainer& teeth);

};

