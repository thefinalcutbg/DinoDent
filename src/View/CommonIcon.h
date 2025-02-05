#pragma once

class QPixmap;

namespace CommonIcon
{
	enum Type { NOICON, NHIF, HIS, HISGRAY, PHIF, PATIENT, BDAY, AMBLIST, PRESCR, PERIO, INVOICE, CALENDAR, PRINT, MAX_SIZE };
	const QPixmap& getPixmap(CommonIcon::Type t);
}