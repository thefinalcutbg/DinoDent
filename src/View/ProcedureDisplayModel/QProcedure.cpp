#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Tooth/ToothUtils.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(Date::toString(p.date))),
	diagnosis(QString::fromStdString(p.diagnosis)),
	tooth(ToothUtils::getToothNumber(p.tooth, p.temp)),
	procedureName(QString::fromStdString(p.name)),
	code(p.code), nzok(p.nzok), price(priceToString(p.price)),
	doctor(QString::fromStdString(p.LPK)) //some fancy function for returning the real doctor name?
{
}
