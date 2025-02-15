#pragma once

#include <QPixmap>
class QColor;

namespace CommonIcon
{
	enum Type { NOICON, NHIF, HIS, HISGRAY, PHIF, PATIENT, BDAY, AMBLIST, PRESCR, PERIO, INVOICE, CALENDAR, PRINT, DATABASE, REFRESH, MAX_SIZE };
	const QPixmap& getPixmap(CommonIcon::Type t);
	QPixmap getIndicator(const QColor& color);
}