#pragma once
#include <array>
#include <optional>
#include "View/Interfaces/IProcedureEditDialog.h"
#include "Model/Dental/Procedure.h"

#include "Model/Validators/ProcedureDateValidator.h"
#include "Model/Validators/ProcedureValidators.h"
#include "Model/Validators/CommonValidators.h"

#include "View/ModalDialogBuilder.h"

class ProcedureEditorPresenter
{

	SurfaceValidator surface_validator;
	BridgeRangeValidator range_validator;
	NotEmptyValidator not_emptyValidator;
	IProcedureEditDialog* view;
	std::optional<Procedure> result;

	ProcedureDateValidator _dateValidator;
	//not present in the view, so state should be saved:
	ProcedureCode m_code;
	bool m_temp;
	int m_tooth;

public:
	ProcedureEditorPresenter(const Procedure& m, const Date& patientBirth);
	std::optional<Procedure> openDialog();
	void setView(IProcedureEditDialog* view);
	void okPressed();
};

