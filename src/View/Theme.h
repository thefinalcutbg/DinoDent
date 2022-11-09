#pragma once
#include <QString>
#include <QColor>
#include <QPainterPath>
namespace Theme
{
	enum class Orientation {Left, Center, Right};

	constexpr int radius{ 20 };
	const QColor mainBackgroundColor(170, 215, 220);
	//const QColor mainBackgroundColor(Qt::white);
	const QColor background(246, 245, 250);
	const QColor sectionBackground(Qt::white);
	const QColor border(225, 225, 225);
	const QColor fontRed(230, 100, 80);
	const QColor fontRedClicked(255, 127, 94);
	const QColor fontTurquoise(2, 127, 128);
	const QColor fontTurquoiseClicked(75, 163, 162);
	const QColor buttonFrame(170, 215, 220);
	const QColor inactiveTabBG(205, 233, 236);
	const QColor inactiveTabBGHover(193, 220, 228);
	const QColor practiceLabel(0, 124, 121);

	QPainterPath getHalfCurvedPath(int width, int heigt);
	QPainterPath getOneCornerCurvedRect(int width, int height);
	QString colorToString(const QColor& color);
	QString getFancyStylesheet();
	QString getPopupMenuStylesheet();

};
