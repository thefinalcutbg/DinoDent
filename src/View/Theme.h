#pragma once
#include <QColor>
#include <QPainterPath>
namespace Theme
{
	enum class Orientation {Left, Center, Right};

	constexpr int radius{ 20 };
	const QColor background(246, 245, 250);
	const QColor sectionBackground(Qt::white);
	const QColor border(225, 225, 225);
	const QColor fontRed(230, 100, 80);
	const QColor fontRedClicked(255, 127, 94);
	const QColor fontTurquoise(2, 127, 128);
	const QColor fontTurquoiseClicked(75, 163, 162);

	QPainterPath getHalfCurvedPath(int width, int heigt);


};
