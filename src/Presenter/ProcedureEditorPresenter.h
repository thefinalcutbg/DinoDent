#pragma once
#include <optional>
#include "View/Interfaces/IProcedureEditDialog.h"
#include "Model/Dental/Procedure.h"

#include "Model/Validators/ProcedureDateValidator.h"



class ProcedureEditorPresenter
{

	//NotEmptyValidator not_emptyValidator;
	IProcedureEditDialog* view;
	std::optional<Procedure> result;

	ProcedureDateValidator _dateValidator;
	//not present in the view, so state should be saved:
	ProcedureCode m_code;
	ToothIndex m_tooth_index;
	int m_hisIndex;

public:
	ProcedureEditorPresenter(const Procedure& m, const Date& patientBirth);
	std::optional<Procedure> openDialog();
	void setView(IProcedureEditDialog* view);
	void okPressed();
};

