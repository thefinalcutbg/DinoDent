#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "View/ListView/ToothPaintDevices/PaintHint.h"


class ToothHintCreator
{
public:
	BridgesPaintHint statusToUIBridge(std::array<Tooth, 32>& teeth);
	ToothPaintHint getToothHint(const Tooth& tooth);
	std::array<ToothPaintHint, 32> getTeethHint(const std::array<Tooth, 32>& teeth);

};

