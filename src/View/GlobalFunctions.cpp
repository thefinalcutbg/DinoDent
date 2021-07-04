#include "GlobalFunctions.h"

#include "QString"
#include <cmath>

QString priceToString(double price)
{
	double intPart;
	return std::modf(price, &intPart) == 0.0 ?
		QString::number(price) + " лв." :
		QString::number(price, 'f', 2) + " лв.";
}
