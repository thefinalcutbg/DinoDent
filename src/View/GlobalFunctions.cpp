#include "GlobalFunctions.h"
#include <string>
#include "QString"
#include <QUrl>
#include <cmath>
#include <QTableView>
#include <QHeaderView>
#include "Model/Date.h"
#include <QApplication>

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

    return roundDouble(price) + QChar(0x00A0) + getPriceSuffix(year);
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

QString linkify(const QString& plain)
{
	static const QRegularExpression re(R"((https?://\S+|www\.\S+))");

	QString out;
	int last = 0;

	auto it = re.globalMatch(plain);

	while (it.hasNext())
	{
		auto m = it.next();

		out += plain.mid(last, m.capturedStart() - last).toHtmlEscaped();

		QString raw = m.captured(0);
		QUrl url = QUrl::fromUserInput(raw);

		out += QString("<a href=\"%1\">%2</a>")
			.arg(url.toString(QUrl::FullyEncoded),
				raw.toHtmlEscaped());

		last = m.capturedEnd();
	}

	out += plain.mid(last).toHtmlEscaped();
	return out;
}

QString priceRangeToString(const std::pair<double, double> priceRange)
{
    static const bool doublePricing = Date::currentDate() < Date(8, 8, 2026);

    constexpr double EUR_TO_BGN = 1.95583;

    auto levAmountToString = [&](double eur) -> QString {
        return roundDouble(eur * EUR_TO_BGN);
    };

    auto levToString = [&](double eur) -> QString {
        return levAmountToString(eur) + QChar(0x00A0) + "лв.";
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
