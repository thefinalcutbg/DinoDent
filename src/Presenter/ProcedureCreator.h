#pragma once

#include <map>
#include <vector>

#include "Model/Dental/Procedure.h"
#include "Model/Dental/Tooth.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureInput.h"

class ProcedureCreator
{

	IProcedureInput* view{ nullptr };

	ProcedureCode m_code;
	std::map<ProcedureType, Diagnosis> diag_map;

	const std::vector<const Tooth*> m_selectedTeeth;

	NotEmptyValidator notEmpty_validator;
	Diagnosis getDiagnosis(const Tooth* tooth, ProcedureType type);
	RestorationData autoSurfaces(const Tooth& tooth);
	ConstructionRange autoRange(const std::vector<const Tooth*> m_selectedTeeth, ProcedureCode code);	

public:
	ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth);

	void setView(IProcedureInput* view);

	void diagnosisChanged(int idx);

	void setProcedureCode(const ProcedureCode& m, bool nhif, double value);

	std::vector<Procedure> getProcedures();
	
};

