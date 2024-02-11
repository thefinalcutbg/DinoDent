#pragma once

#include "Model/Dental/ToothUtils.h"
#include "Model/Validators/ProcedureDateValidator.h"
#include "ProcedureCreator.h"
#include "View/Interfaces/IProcedureDialog.h"
#include "Model/Dental/ProcedureListElement.h"

struct AmbList;

class ProcedureDialogPresenter
{
	int currentIndex{ -1 };

	std::vector<const Tooth*> selectedTeeth;

	const AmbList& ambList;

	Date patientTurns18;
	bool pregnancyAllowed{ false };

	ProcedureCreator procedure_creator;

	IProcedureDialog* view;

	std::vector<Procedure> procedures;

	std::vector<ProcedureListElement> procedureList;

	ProcedureDateValidator date_validator;

	bool noProcedureSelected;

	void sortProcedures();

public:

	ProcedureDialogPresenter
	(
		const AmbList& ambSheet,
		const std::vector<const Tooth*>& selectedTeeth,
		const Date& patientTurns18,
		bool pregnancyAllowed
	);

	void setView(IProcedureDialog* view);
	void procedureDateChanged(const Date& date);
	void indexChanged(int index);
	void favouriteClicked(int index);
	void formAccepted();

	std::vector<Procedure> openDialog();

};

