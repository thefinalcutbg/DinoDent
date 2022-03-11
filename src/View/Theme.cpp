
#include "Theme.h"


QPainterPath Theme::getHalfCurvedPath(int width, int height)
{
	QPainterPath path;

	path.moveTo(radius, 0);
	path.lineTo(width - 1, 0);
	path.lineTo(width - 1, height - 1);
	path.lineTo(radius, height - 1);
	path.arcTo(0, height - radius - 1, radius, radius, -90, -90);
	path.lineTo(0, radius);
	path.arcTo(0, 0, radius, radius, 180, -90);
	path.lineTo(radius, 0);

	return path;
}
