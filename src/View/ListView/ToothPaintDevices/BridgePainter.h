#pragma once

#include <array>
#include <functional>

#include <QPixmap>
#include <QPainter>
#include "Model/Tooth/ToothUtils.h""
#include "PaintHint.h"
#include "SpriteSheets.h"


class BridgePainter
{
	QRect molarCrop;
	QRect molarPaint;

	ToothUtils utilities;
	QPixmap getOverlay(const QPixmap& source);
	QPixmap* paintHalf(std::array<BridgeAppearenceTuple, 16> bridgeHalf, bool lower);

public:
	BridgePainter();
	std::pair<QPixmap*, QPixmap*> getBridgePair(const std::array<BridgeAppearenceTuple, 32>& bridges);

};

