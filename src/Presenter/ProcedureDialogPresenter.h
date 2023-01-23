#pragma once

#include "Model/Dental/ToothUtils.h"
#include "Model/Validators/ProcedureDateValidator.h"

#include "GeneralProcedurePresenter.h"
#include "ToothProcedurePresenter.h"
#include "ObturationPresenter.h"
#include "CrownPresenter.h"
#include "ProcedureRangePresenter.h"
#include "View/Interfaces/IProcedureDialog.h"

struct AmbList;

class ProcedureDialogPresenter
{
	int currentIndex{ -1 };

	std::vector<Tooth*> selectedTeeth;

	const AmbList& ambList;

	Date patientTurns18;
	bool pregnancyAllowed{ false };

	GeneralProcedurePresenter general_presenter;
	ToothProcedurePresenter toothNonSpecific_presenter;
	ObturationPresenter obt_presenter;
	CrownPresenter crown_presenter;
	ExtractionPresenter extr_presenter;
	EndoPresenter endo_presenter;
	ImplantPresenter impl_presenter;
	ProcedureRangePresenter fiber_presenter;
	ToothProcedurePresenter crownRemove_presenter;
	ToothProcedurePresenter postRemove_presenter;
	ProcedureRangePresenter bridgeSplintRemove_presenter;

	std::array<AbstractSubPresenter*, 11> presenters_ptr;
	AbstractSubPresenter* current_m_presenter;

	IProcedureDialog* view;

	std::vector<Procedure> procedures;

	std::vector<ProcedureTemplate> procedureList;

	ProcedureDateValidator date_validator;


	bool noProcedureSelected;

	public:

		ProcedureDialogPresenter
		(
			const AmbList& ambSheet,
			const std::vector<Tooth*>& selectedTeeth,
			const Date& patientTurns18,
			bool pregnancyAllowed
		);

		void setView(IProcedureDialog* view);
		void procedureDateChanged(const Date& date);
		void indexChanged(int index);
		void formAccepted();
		std::vector<Procedure> openDialog();
};

