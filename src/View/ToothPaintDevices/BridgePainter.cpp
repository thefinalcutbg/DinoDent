#include "BridgePainter.h"
#include "Model/Tooth/ToothUtils.h"


BridgePainter::BridgePainter()
{
}

QPixmap BridgePainter::getOverlay(const QPixmap& source)
{
	auto overlay = source;
	QPainter painter(&overlay);
	painter.setOpacity(0.3);
	painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	painter.fillRect(source.rect(), Qt::green);
	painter.end();
	return overlay;
}

QPixmap* BridgePainter::paintHalf(std::array<std::tuple<BridgeAppearance, bool>, 16> bridgeHalf, bool lower)
{
	QPixmap* currentTexture;

	lower ?
	currentTexture = SpriteSheets::container().getLowerBridge()
	:
	currentTexture = SpriteSheets::container().getUpperBridge();

	QPixmap bridge(2280, 140);
	bridge.fill(Qt::transparent);
	QPainter painter(&bridge);
	painter.setOpacity(0.8);

	int height = 140;
	int width = 0; //180 or 120

	int posY = 0;
	int posX = 0; //increment, depending on position;

	bool bridgeHasBegun = 0;

	for (int i = 0; i < bridgeHalf.size(); i++)
	{
		auto [bridgeApp, madeByMe] = bridgeHalf[i];

		(i < 3 || i > 12)
			? width = 180 : width = 120;

		QPixmap texture;

		if (bridgeApp == BridgeAppearance::Middle)
		{
			QRect crop(posX, posY, width, height);
			texture = currentTexture->copy(crop);
			painter.drawPixmap(posX, posY, texture);

			if (madeByMe)
				painter.drawPixmap(posX, posY, getOverlay(texture));

		}
		else if (bridgeApp == BridgeAppearance::Terminal)
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

			texture = currentTexture->copy(upRect);

				painter.drawPixmap(posX, posY, texture);
			if (madeByMe) 
				painter.drawPixmap(posX, posY, getOverlay(texture));


			texture = currentTexture->copy(downRect);

				painter.drawPixmap(posX + width / 2, posY, texture);
			if (madeByMe)
				painter.drawPixmap(posX + width / 2, posY, getOverlay(texture));

			bridgeHasBegun = !bridgeHasBegun;
		}

		posX += width;
	}

	return new QPixmap(bridge);
}


std::pair<QPixmap*, QPixmap*> BridgePainter::getBridgePair(const BridgesPaintHint& bridges)
{
	std::array<std::tuple<BridgeAppearance, bool>, 16> upperArrayBridge;
	std::array<std::tuple<BridgeAppearance, bool>, 16> lowerArrayBridge;

	for (int i = 0; i < 16; i++) {
		upperArrayBridge[i] = bridges.paintHints[i];
		lowerArrayBridge[i] = bridges.paintHints[i + 16];
	}

	return std::make_pair<QPixmap*, QPixmap*>(paintHalf(upperArrayBridge, 0), paintHalf(lowerArrayBridge, 1));

}
