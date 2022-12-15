#pragma once

class QPixmap;

namespace Zodiac
{
	void initialize();
	QPixmap* getPixmap(int day, int month);
}