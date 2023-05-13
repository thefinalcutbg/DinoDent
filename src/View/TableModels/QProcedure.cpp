#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"
#include "Model/Dental/Procedure.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.diagnosis.getFullDiagnosis())),
	tooth(ToothUtils::getNhifNumber(p.tooth, p.temp, p.hyperdontic).c_str()),
	procedureName(QString::fromStdString(p.code.name())),
	code(p.code.oldCode()),
	fsource(p.financingSource),
	ksmp(QString::fromStdString(p.financingSource == FinancingSource::NHIF ? p.code.ksmp() : "")),
	doctor(QString::fromStdString(User::getNameFromLPK(p.LPK))),
	notes(QString::fromStdString(p.notes)),
	db_source(p.db_source)
{
	if (tooth == "99") tooth.clear();

	if (p.his_index) db_source = Procedure::HIS;

	if (std::holds_alternative<ConstructionRange>(p.result)) {
		auto& range = std::get<ConstructionRange>(p.result);
		tooth += QString::number(ToothUtils::getToothNumber(range.tooth_begin, false));
		tooth += "-";
		tooth += QString::number(ToothUtils::getToothNumber(range.tooth_end, false));
	}
}
