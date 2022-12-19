#include "Zodiac.h"
#include <QPixmap>
QString z_str[13]{ "♑︎","♒︎","♋︎","♈︎","♉︎","♊︎","♋︎","♌︎","♍︎","♎︎","♏︎","♐︎","♑︎" };



QPixmap* zodiac_png[13] {nullptr};

void Zodiac::initialize()
{
	static QString path = ":/zodiac/zodiac_";

	for (int i = 0; i < 12; i++)
	{
		zodiac_png[i] = new QPixmap(path + QString::number(i) + ".png");
	}

	zodiac_png[12] = zodiac_png[0]; //december capricorn case
}

QPixmap* Zodiac::getPixmap(int day, int month)
{
	if (month < 1 || month > 12) return nullptr;

	static int zodiacDay[12] {20, 19, 21, 20, 21, 21, 23, 23, 23, 23, 22, 22};

	int index = month-1;

	if (day < zodiacDay[index]) return zodiac_png[index];

	return zodiac_png[month];

}

QString Zodiac::getString(int day, int month)
{
	if (month < 1 || month > 12) return nullptr;

	static int zodiacArr[12]{ 20, 19, 21, 20, 21, 21, 23, 23, 23, 23, 22, 22 };

	int index = month - 1;

	if (day < zodiacArr[index]) return z_str[index];

	return z_str[month];
}


