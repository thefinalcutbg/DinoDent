#pragma once
#include <array>
#include <optional>
#include "View/Interfaces/IProcedureEditDialog.h"
#include "Model/Dental/Procedure.h"

#include "Model/Validators/ProcedureDateValidator.h"
#include "Model/Validators/ProcedureValidators.h"

#include "View/ModalDialogBuilder.h"
#include "Presenter/ImplantPresenter.h"
#include "Presenter/CrownPresenter.h"
#include "Presenter/ObturationPresenter.h"
#include "Presenter/EditorPresenter.h"

class ProcedureEditorPresenter
{
	//required for some procedures (surfaces, range, etc)
	AbstractUIElement* m_validatableElement;

	EditorPresenter commonEditorPresenter;

	IProcedureEditDialog* view;
	Procedure m_procedure;
	std::optional<Procedure> result;

	SurfaceValidator _surfValidator;
	BridgeRangeValidator _bridgeValidator;
	NotEmptyValidator _emptyValidator;
	ProcedureDateValidator _dateValidator;

public:
	ProcedureEditorPresenter(const Procedure& m, const Date& patientBirth);
	std::optional<Procedure> openDialog();
	void setView(IProcedureEditDialog* view);
	void okPressed();
};

