#include "QProcedure.h"

#include "Model/User.h"
#include "Model/Dental/Procedure.h"

QProcedure::QProcedure(const Procedure& p) :
	date(QString::fromStdString(p.date.toBgStandard())),
	diagnosis(QString::fromStdString(p.diagnosis.getDiagnosisText())),
	tooth(p.getToothString().c_str()),
	procedureName(QString::fromStdString(p.code.name())),
    nhifCode(p.financingSource == FinancingSource::NHIF ? QString::number(p.code.nhifCode()) : "" ),
    ACHICode(QString::fromStdString(p.code.ACHICode())),
    fsource(p.financingSource),
	doctor(QString::fromStdString(User::getNameFromLPKorRHIF(p.LPK))),
	notes(QString::fromStdString(p.notes)),
	db_source(p.db_source)
{
	if (p.his_index) db_source = Procedure::HIS;
}
