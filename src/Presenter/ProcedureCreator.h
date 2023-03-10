#pragma once

#include <map>
#include <vector>

#include "Model/Dental/Procedure.h"
#include "Model/Dental/Tooth.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureDialog.h"
#include "View/Interfaces/IProcedureInput.h"

class ProcedureCreator
{

	IProcedureInput* view{ nullptr };

	ProcedureCode m_code;
	std::map<ProcedureType, std::string> diag_map;

	const std::vector<const Tooth*> m_selectedTeeth;

	NotEmptyValidator notEmpty_validator;
	SurfaceValidator surface_validator;
	BridgeRangeValidator range_validator;

	std::string restorationDiagnosis(const Tooth& tooth);
	std::string extractionDiagnosis(const Tooth& tooth);
	std::string endodonticDiagnosis(const Tooth& tooth);
	std::string crownDiagnosis(const Tooth& tooth);
	std::string implantDiagnosis(const Tooth& tooth);
	std::array<bool, 6> autoSurfaces(const Tooth& tooth);
	ConstructionRange getBridgeRange(const std::vector<const Tooth*> m_selectedTeeth);
	std::string bridgeOrFiberDiagnosis(const std::vector<const Tooth*> selectedTeeth, const ConstructionRange& range);
	
	

public:
	ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth);


	void setView(IProcedureInput* view);

	void diagnosisTextChanged(std::string text) { diag_map[m_code.type()] = text; };

	void setProcedureCode(const ProcedureCode& m);

	bool isValid();

	virtual std::vector<Procedure> getProcedures();
	
};

