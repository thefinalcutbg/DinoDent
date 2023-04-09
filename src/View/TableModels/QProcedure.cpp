#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"
#include "Model/Dental/Procedure.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.getFullDiagnosis())),
	tooth(ToothUtils::getNhifNumber(p.tooth, p.temp, p.hyperdontic).c_str()),
	procedureName(QString::fromStdString(p.code.name())),
	code(p.code.nhifCode()), fsource(p.financingSource),
	ksmp(QString::fromStdString(p.code.ksmp())),
	doctor(QString::fromStdString(User::getNameFromLPK(p.LPK))),
	notes(QString::fromStdString(p.notes))
{
	if (tooth == "99") tooth.clear();

	if (p.isRangeSpecific()) {
		tooth.clear();
		auto& range = std::get<ConstructionRange>(p.result);
		tooth += QString::number(ToothUtils::getToothNumber(range.tooth_begin, false));
		tooth += "-";
		tooth += QString::number(ToothUtils::getToothNumber(range.tooth_end, false));
	}
}
