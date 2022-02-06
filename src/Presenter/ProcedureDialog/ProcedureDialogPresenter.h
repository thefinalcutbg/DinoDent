#pragma once

#include "Model/Tooth/ToothUtils.h"
#include "Model/Validator/ProcedureDateValidator.h"

#include "SubPresenters/GeneralProcedurePresenter.h"
#include "SubPresenters/ToothProcedurePresenter.h"
#include "SubPresenters/ObturationPresenter.h"
#include "SubPresenters/CrownPresenter.h"
#include "SubPresenters/ExtractionPresenter.h"
#include "SubPresenters/EndoPresenter.h"
#include "SubPresenters/ImplantPresenter.h"
#include "SubPresenters/FiberSplintPresenter.h"



#include "View/ProcedureDialog/IProcedureDialog.h"

class ToothContainer;

class ProcedureDialogPresenter
{
	int currentIndex{ -1 };

	std::vector<Tooth*> selectedTeeth;

	GeneralProcedurePresenter general_presenter;
	ToothProcedurePresenter toothNonSpecific_presenter;
	ObturationPresenter obt_presenter;
	CrownPresenter crown_presenter;
	ExtractionPresenter extr_presenter;
	EndoPresenter endo_presenter;
	ImplantPresenter impl_presenter;
	FiberSplintPresenter fiber_presenter;
	ToothProcedurePresenter crownRemove_presenter;

	std::array<AbstractSubPresenter*, 9> presenters_ptr;
	AbstractSubPresenter* current_m_presenter;

	IProcedureDialog* view;
	ICommonFields* common_fields;

	std::vector<Procedure> procedures;

	std::vector<ProcedureTemplate> procedureList;
	const ToothContainer* teeth;

	ProcedureDateValidator date_validator;


	bool _errorState;

	public:

		ProcedureDialogPresenter
		(
			const std::vector<Tooth*>& selectedTeeth,
			const ToothContainer& teeth,
			const Date& ambListDate,
			const Date& patientBirth,
			bool fullCoverage,
			int specialty
		);

		void setView(IProcedureDialog* view);
		
		void indexChanged(int index);
		void formAccepted();
		std::vector<Procedure> openDialog();
};

