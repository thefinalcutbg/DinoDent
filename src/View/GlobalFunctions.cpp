#include "GlobalFunctions.h"
#include <string>
#include "QString"
#include <cmath>
#include <QTableView>
#include <QHeaderView>
#include "Model/Date.h"

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
    static const bool doublePricing = Date::currentDate() < Date(8, 8, 2026);

    constexpr double EUR_TO_BGN = 1.95583;

    auto levAmountToString = [&](double eur) -> QString {
        return roundDouble(eur * EUR_TO_BGN);
    };

    auto levToString = [&](double eur) -> QString {
        return levAmountToString(eur) + " лв.";
    };

    if(!priceRange.first && !priceRange.second){
        return priceToString(0, 2026);
    }

    // Single price
    if (priceRange.first == priceRange.second) {
        QString result = priceToString(priceRange.first, 2026);

        if (doublePricing) {
            result += " / " + levToString(priceRange.first);
        }

        return result;
    }

    QString result = roundDouble(priceRange.first);
    result += "-";
    result += priceToString(priceRange.second, 2026);

    if (!doublePricing) {
        return result;
    }

    QString levRange = levAmountToString(priceRange.first);
    levRange += "-";
    levRange += levToString(priceRange.second);

    result += " / " + levRange;
    return result;
}
