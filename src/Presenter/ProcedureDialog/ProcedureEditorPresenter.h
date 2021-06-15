#pragma once
#include "View/ProcedureEditDialog/IProcedureEditDialog.h"
#include "Model/Manipulation/Manipulation.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/ProcedureDialog/SubPresenters/ImplantPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/CrownPresenter.h"
#include "Presenter/ProcedureDialog/SubPresenters/ObturationPresenter.h"
class ProcedureEditorPresenter
{
	IProcedureEditDialog* view;
	Manipulation m;

	BridgeRangeValidator bridgeValidator;
	

public:
	ProcedureEditorPresenter(const Manipulation& m);
	Manipulation openDialog();
	void setView(IProcedureEditDialog* view);
	void okPressed();
};

