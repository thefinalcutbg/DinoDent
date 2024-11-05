#pragma once

#include "Model/Dental/ToothUtils.h"
#include "Model/Validators/ProcedureDateValidator.h"
#include "ProcedureCreator.h"
#include "View/Interfaces/IProcedureDialog.h"
#include "Model/Dental/ProcedureListElement.h"

struct AmbList;

class ProcedureDialogPresenter
{
	static inline int sectionIndex = -1;

	std::vector<const Tooth*> selectedTeeth;

	const AmbList& ambList;

	Date patientTurns18;
	Date procedureDate;
	bool pregnancyAllowed{ false };
	bool favourites_changed{ false };

	ProcedureCreator procedure_creator;

	IProcedureDialog* view;

	std::vector<Procedure> result;

	ProcedureList procedureList;

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

	void setView(IProcedureDialog* view);
	void procedureDateChanged(const Date& date);
	void sectionChanged(int index);
	void setCode(ProcedureCode code, bool nhif);
	void favouriteClicked(const std::string& code);
	void formAccepted();

	std::vector<Procedure> openDialog();

	~ProcedureDialogPresenter();

};

