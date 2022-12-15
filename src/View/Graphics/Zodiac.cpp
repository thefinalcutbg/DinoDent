#include "Zodiac.h"
#include <QPixmap>

QPixmap* z_arr[13] {nullptr};

void Zodiac::initialize()
{
	static QString path = ":/zodiac/zodiac_";

	for (int i = 0; i < 12; i++)
	{
		z_arr[i] = new QPixmap(path + QString::number(i) + ".png");
	}

	z_arr[12] = z_arr[0]; //december capricorn case
}

QPixmap* Zodiac::getPixmap(int day, int month)
{
	if (month < 1 || month > 12) return nullptr;

	static int zodiacArr[12] {20, 19, 21, 20, 21, 21, 23, 23, 23, 23, 22, 22};

	int index = month-1;

	if (day < zodiacArr[index]) return z_arr[index];

	return z_arr[month];

}


