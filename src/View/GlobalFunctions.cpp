#include "GlobalFunctions.h"

#include "QString"
#include <cmath>

QString roundDouble(double number)
{
	double intPart;
	return std::modf(number, &intPart) == 0.0 ?
		QString::number(number) :
		QString::number(number, 'f', 2);
}

QString priceToString(double price) { return roundDouble(price) + u8" лв."; }

