#pragma once

class QPixmap;
#include <QString>

namespace Zodiac
{
	void initialize();
	QPixmap* getPixmap(int day, int month);
	QString getString(int day, int month);
}