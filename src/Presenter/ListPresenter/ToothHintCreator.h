#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "View/ListView/ToothPaintDevices/PaintHint.h"


class ToothHintCreator
{
public:
	std::array<BridgeAppearenceTuple, 32> statusToUIBridge(std::array<Tooth, 32>& teeth);
	PaintHint getToothHint(const Tooth& tooth);
	std::array<PaintHint, 32> getTeethHint(const std::array<Tooth, 32>& teeth);

};

