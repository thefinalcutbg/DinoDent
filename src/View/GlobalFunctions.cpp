#include "GlobalFunctions.h"
#include <string>
#include "QString"
#include <cmath>

QString roundDouble(double number)
{
	double intPart;
	return std::modf(number, &intPart) == 0.0 ?
		QString::number(number) :
		QString::number(number, 'f', 2);
}

QString priceToString(double price) { return roundDouble(price) + " лв."; }

QString formatDoubleWithDecimal(const double& price) //adding the dot in case of integer
{
	return QString::number(price, 'f', 2);
};

