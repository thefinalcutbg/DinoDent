#pragma once
#include "Model/Dental/ToothUtils.h"
#include "View/Graphics/PaintHint.h"

class ToothContainer;

namespace ToothHintCreator
{

	ToothPaintHint getToothHint(const Tooth& tooth);
	std::array<ToothPaintHint, 32> getTeethHint(const ToothContainer& teeth);

};

