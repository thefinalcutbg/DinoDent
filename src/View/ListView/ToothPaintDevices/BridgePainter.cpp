#include "BridgePainter.h"

#include <QDebug>

BridgePainter::BridgePainter()
{
}

QPixmap* BridgePainter::paintHalf(std::array<BridgeAppearance, 16> bridgeHalf, bool lower)
{
	QPixmap* currentTexture;

	if (lower) {

		currentTexture = SpriteSheets::container().getLowerBridge();
	}
	else {
		currentTexture = SpriteSheets::container().getUpperBridge();
	}

	QPixmap bridge(2280, 140);
	bridge.fill(Qt::transparent);
	QPainter painter(&bridge);
	painter.setOpacity(0.8);

	int height = 140;
	int width; //180 or 120

	int posY = 0;
	int posX = 0; //increment, depending on position;

	bool bridgeHasBegun = 0;

	for (int i = 0; i < bridgeHalf.size(); i++)
	{

		if (utilities.getToothType(i) == ToothType::Molar)
		{
			width = 180;
		}
		else { width = 120; }

		if (bridgeHalf[i] == BridgeAppearance::Middle)
		{
			QRect crop(posX, posY, width, height);
			painter.drawPixmap(posX, posY, currentTexture->copy(crop));
		}
		else if (bridgeHalf[i] == BridgeAppearance::Terminal)
		{
			QRect upRect;
			QRect downRect;

			if (!bridgeHasBegun)
			{
				upRect = QRect(posX, posY + 150, width / 2, height);
				downRect = QRect(posX + (width / 2), posY, width / 2, height);
			}
			else
			{
				upRect = QRect(posX, posY, width / 2, height);
				downRect = QRect(posX + (width / 2), posY + 150, width / 2, height);
			}

			painter.drawPixmap(posX, posY, currentTexture->copy(upRect));
			painter.drawPixmap(posX + width / 2, posY, currentTexture->copy(downRect));

			bridgeHasBegun = !bridgeHasBegun;
		}

		posX += width;
	}

	return new QPixmap(bridge);
}


std::pair<QPixmap*, QPixmap*> BridgePainter::getBridgePair(std::array<BridgeAppearance, 32> bridgeArray)
{
	std::array<BridgeAppearance, 16> upperArrayBridge;
	std::array<BridgeAppearance, 16> lowerArrayBridge;

	for (int i = 0; i < 16; i++) {
		upperArrayBridge[i] = bridgeArray[i];
		lowerArrayBridge[i] = bridgeArray[i + 16];
	}

	return std::make_pair<QPixmap*, QPixmap*>(paintHalf(upperArrayBridge, 0), paintHalf(lowerArrayBridge, 1));

}
