#include "HisSnapshot.h"
#include "Model/Dental/Procedure.h"

HisSnapshot::HisSnapshot(const Procedure& p, const ToothContainer& t) :
	date(p.date),
	procedure_name(p.code.name()),
	procedure_diagnosis(p.diagnosis.getDiagnosisText()),
	procedure_note(p.notes),
	financing(p.financingSource),
	teeth(t)
{ 
	auto teethPtr = p.applyProcedure(teeth);

	affected_teeth.reserve(teethPtr.size());

	for (auto& tooth : teethPtr) {
		affected_teeth.push_back(tooth->index());
	}
}

HisSnapshot::HisSnapshot(const ToothContainer& t, Date& date) :
	date(date),
	procedure_name("Изходен статус"),
	financing(FinancingSource::None),
	teeth(t)
{

}
