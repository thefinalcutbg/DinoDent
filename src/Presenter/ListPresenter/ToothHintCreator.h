#pragma once
#include "Model/Tooth/ToothUtils.h"
#include "View/ListView/ToothPaintDevices/PaintHint.h"


class ToothHintCreator
{
public:
	PaintHint getToothHint(const Tooth& tooth);

};

