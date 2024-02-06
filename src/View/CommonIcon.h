#pragma once

class QPixmap;

namespace CommonIcon
{
	enum Type { NOICON, NHIF, HIS, HISGRAY, PHIF, BDAY, AMBLIST, PRESCR, PERIO, INVOICE, MAX_SIZE };
	const QPixmap& getPixmap(CommonIcon::Type t);
}