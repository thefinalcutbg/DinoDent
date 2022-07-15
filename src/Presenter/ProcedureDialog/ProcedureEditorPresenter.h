#pragma once
#include <array>
#include <optional>
#include "View/ProcedureEditDialog/IProcedureEditDialog.h"
#include "Model/Procedure/Procedure.h"

#include "Model/Validator/ProcedureDateValidator.h"
#include "Model/Validator/ProcedureValidators.h"

#include "View/ModalDialogBuilder.h"
#include "Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/EditorPresenter.h"

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

