#include "QProcedure.h"

#include "View/GlobalFunctions.h"
#include "Model/Dental/ToothUtils.h"
#include "Model/User.h"
#include "Model/Dental/Procedure.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.diagnosis.getFullDiagnosis())),
	tooth(p.getToothString().c_str()),
	procedureName(QString::fromStdString(p.code.name())),
	code(p.code.oldCode()),
	fsource(p.financingSource),
	ksmp(QString::fromStdString(p.financingSource == FinancingSource::NHIF ? p.code.ksmp() : "")),
	doctor(QString::fromStdString(User::getNameFromLPK(p.LPK))),
	notes(QString::fromStdString(p.notes)),
	db_source(p.db_source)
{
	if (p.his_index) db_source = Procedure::HIS;
}
