#pragma once
#include "Model/Dental/Procedure.h"
#include "Model/Dental/Tooth.h"
#include "View/Interfaces/ICommonFields.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Validators/CommonValidators.h"
#include "View/Interfaces/IProcedureDialog.h"

#include <vector>

class ProcedureCreator
{

	ICommonFields* view{ nullptr };

	ProcedureCode m_code;
	std::string m_diagnosis;

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
	std::string bridgeOrFiberDiagnosis(const std::vector<const Tooth*> selectedTeeth, ConstructionRange& range);
	
	

public:
	ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth);


	void setView(ICommonFields* view);

	void diagnosisTextChanged(std::string text) { m_diagnosis = text; };

	void setProcedureCode(const ProcedureCode& m);

	bool isValid();

	virtual std::vector<Procedure> getProcedures();
	
};

