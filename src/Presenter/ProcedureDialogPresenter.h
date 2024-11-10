#pragma once

#include "Model/Dental/ToothUtils.h"
#include "Model/Validators/ProcedureDateValidator.h"
#include "ProcedureCreator.h"

#include "Presenter/ProcedureListPresenter.h"

struct AmbList;
class ProcedureDialog;

class ProcedureDialogPresenter
{
	std::vector<const Tooth*> selectedTeeth;

	const AmbList& ambList;

	Date patientTurns18;
	Date procedureDate;
	bool pregnancyAllowed{ false };

	ProcedureCreator procedure_creator;
	ProcedureListPresenter list_presenter;
	ProcedureDialog* view;

	std::vector<Procedure> result;

	ProcedureDateValidator date_validator;

	void refreshNhifList();

public:

	ProcedureDialogPresenter
	(
		const AmbList& ambSheet,
		const std::vector<const Tooth*>& selectedTeeth,
		const Date& patientTurns18,
		bool pregnancyAllowed
	);

	void setView(ProcedureDialog* view);
	void procedureDateChanged(const Date& date);
	void setCode(ProcedureCode code, bool nhif, double price);
	void formAccepted();

	std::vector<Procedure> openDialog();

	~ProcedureDialogPresenter();

};

