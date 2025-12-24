#include "GlobalFunctions.h"
#include <string>
#include "QString"
#include <cmath>
#include <QTableView>
#include <QHeaderView>


QString roundDouble(double number)
{
	double intPart;
	return std::modf(number, &intPart) == 0.0 ?
		QString::number(number) :
		QString::number(number, 'f', 2);
}

QString getPriceSuffix(int year)
{
	return year < 2026 ? "лв." : "€";
}

QString priceToString(double price, int year) {

	return roundDouble(price) + getPriceSuffix(year);
}

QString formatDoubleWithDecimal(const double& price) //adding the dot in case of integer
{
	return QString::number(price, 'f', 2);
}



void setTableViewDefaults(QTableView* view)
{
	view->verticalHeader()->setHidden(true);
	view->verticalHeader()->setDefaultSectionSize(20);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	view->horizontalHeader()->setStretchLastSection(true);
	//view->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	view->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	view->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
}



QString priceRangeToString(const std::pair<double, double> priceRange)
{
    if(priceRange.first == priceRange.second){
        return priceToString(priceRange.first, 2026);
    }

    QString result = roundDouble(priceRange.first);
    result += "-";
    result += priceToString(priceRange.second, 2026);

    return result;
}
