
#include "Theme.h"


QPainterPath Theme::getHalfCurvedPath(int width, int height)
{
	QPainterPath path;

	path.moveTo(radius, 0);
	path.lineTo(width, 0);
	path.lineTo(width, height);
	path.lineTo(radius, height);
	path.arcTo(0, height - radius, radius, radius, -90, -90);
	path.lineTo(0, radius);
	path.arcTo(0, 0, radius, radius, 180, -90);
	path.lineTo(radius, 0);

	return path;
}

QString Theme::getRGBStringFromColor(const QColor& color)
{
	QString style = " rgb(%1, %2, %3);";
	return QString(style.arg(color.red()).arg(color.green()).arg(color.blue()));
}
