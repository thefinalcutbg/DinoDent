#pragma once


#include "Model/Tooth/ToothUtils.h"

#include "Model/Validator/ManipulationValidators.h"
#include "View/uiComponents/AbstractLineEdit.h"

#include "SubPresenters/ObturationPresenter.h"
#include "SubPresenters/CrownPresenter.h"
#include "SubPresenters/ExtractionPresenter.h"
#include "SubPresenters/EndoPresenter.h"
#include "SubPresenters/ImplantPresenter.h"
#include "View/ProcedureDialog/IProcedureDialog.h"


class ProcedureDialogPresenter
{
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

	std::vector<Manipulation> manipulations;

	const std::vector<ManipulationTemplate> manipulationList;
	const std::array<Tooth, 32>* teeth;

	DateValidator date_validator;

	int currentIndex{-1};
	bool errorState;

	public:
		ProcedureDialogPresenter(const std::vector<ManipulationTemplate>& mList, const std::vector<Tooth*>& selectedTeeth, const std::array<Tooth, 32>& teeth, const Date& ambListDate);
		void setView(IProcedureDialog* view);
		
		void indexChanged(int index);
		void formAccepted();
		std::vector<Manipulation> openDialog();
};

