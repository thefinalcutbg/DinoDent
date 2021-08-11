#pragma once

#include "Model/Tooth/ToothUtils.h"
#include "Model/Validator/ProcedureDateValidator.h"

#include "SubPresenters/ObturationPresenter.h"
#include "SubPresenters/CrownPresenter.h"
#include "SubPresenters/ExtractionPresenter.h"
#include "SubPresenters/EndoPresenter.h"
#include "SubPresenters/ImplantPresenter.h"

#include "View/ProcedureDialog/IProcedureDialog.h"

class ToothContainer;

class ProcedureDialogPresenter
{
	int currentIndex{ -1 };

	std::vector<Tooth*> selectedTeeth;

	GeneralMPresenter general_presenter;
	TeethMPresenter any_teeth_presenter;
	ObturationPresenter obt_presenter;
	CrownPresenter crown_presenter;
	ExtractionPresenter extr_presenter;
	EndoPresenter endo_presenter;
	ImplantPresenter impl_presenter;

	std::array<GeneralMPresenter*, 7> presenters_ptr;
	GeneralMPresenter* current_m_presenter;

	IProcedureDialog* view;
	ICommonFields* common_fields;

	std::vector<Procedure> manipulations;

	std::vector<ProcedureTemplate> manipulationList;
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
			bool unfavourable,
			int specialty
		);

		void setView(IProcedureDialog* view);
		
		void indexChanged(int index);
		void formAccepted();
		std::vector<Procedure> openDialog();
};

