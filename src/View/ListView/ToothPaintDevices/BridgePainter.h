#pragma once

#include <array>
#include <functional>

#include <QPixmap>
#include <QPainter>
#include "Model/Tooth/ToothUtils.h""
#include "Model/Tooth/Tooth.h"
#include "SpriteSheets.h"




class BridgePainter
{
	QRect molarCrop;
	QRect molarPaint;

	ToothUtils utilities;

	QPixmap* paintHalf(std::array<BridgeAppearance, 16> bridgeHalf, bool lower);

public:
	BridgePainter();
	std::pair<QPixmap*, QPixmap*> getBridgePair(std::array<BridgeAppearance, 32> bridgeArray);

};

