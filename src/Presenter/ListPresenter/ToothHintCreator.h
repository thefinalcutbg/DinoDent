#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "View/ListView/ToothPaintDevices/PaintHint.h"

class ToothContainer;

namespace ToothHintCreator
{

	BridgesPaintHint statusToUIBridge(const ToothContainer& teeth);
	ToothPaintHint getToothHint(const Tooth& tooth);
	std::array<ToothPaintHint, 32> getTeethHint(const ToothContainer& teeth);

};

