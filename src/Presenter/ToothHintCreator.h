#pragma once
#include "View/Graphics/PaintHint.h"
#include "Model/Dental/ToothUtils.h"
#include <array>

class ToothContainer;

namespace ToothHintCreator
{

	ToothPaintHint getToothHint(const Tooth& tooth, const std::string& note = std::string{});
	std::array<ToothPaintHint, 32> getTeethHint(const ToothContainer& teeth);

};

