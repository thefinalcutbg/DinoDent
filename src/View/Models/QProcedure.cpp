#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.diagnosis)),
	tooth(ToothUtils::getToothNumber(p.tooth, p.temp)),
	procedureName(QString::fromStdString(p.name)),
	code(p.code), nzok(p.nzok), price(priceToString(p.price)),
	ksmp(QString::fromStdString(p.ksmp)),
	doctor(QString::fromStdString(User::getNameFromLPK(p.LPK)))
{
}