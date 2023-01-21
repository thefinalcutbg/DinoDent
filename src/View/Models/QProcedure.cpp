#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.diagnosis)),
	tooth(ToothUtils::getNhifNumber(p.tooth, p.temp, p.hyperdontic).c_str()),
	procedureName(QString::fromStdString(p.name)),
	code(p.code), fsource(p.financingSource),
	ksmp(QString::fromStdString(p.ksmp)),
	doctor(QString::fromStdString(User::getNameFromLPK(p.LPK))),
	notes(QString::fromStdString(p.notes))
{
	if (tooth == "99") tooth.clear();
}
