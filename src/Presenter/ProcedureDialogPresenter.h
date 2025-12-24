#pragma once

#include "Model/Dental/NhifSheetData.h"
#include "Model/Validators/ProcedureDateValidator.h"
#include "ProcedureCreator.h"

#include "Presenter/ProcedureListPresenter.h"

struct AmbList;
class ProcedureDialog;

class ProcedureDialogPresenter
{

public:
    struct NhifData{
        Date patientTurns18;
        bool pregnancyAllowed;
        NhifSpecificationType specType;
    };

private:
	std::vector<const Tooth*> selectedTeeth;

    std::optional<NhifData> nhifData;

    Date procedureDate;

	ProcedureCreator procedure_creator;
	ProcedureListPresenter list_presenter;
	ProcedureDialog* view;

	std::vector<Procedure> result;

	ProcedureDateValidator date_validator;

	void refreshNhifList();

public:

	ProcedureDialogPresenter
    (
		const std::vector<const Tooth*>& selectedTeeth,
        const Date& procedureDate = Date::currentDate(),
        const std::optional<NhifData>& nhifData = std::optional<NhifData>{}
	);

	void setView(ProcedureDialog* view);
	void procedureDateChanged(const Date& date);
    void setCode(ProcedureCode code, bool nhif);
	void formAccepted();

	std::vector<Procedure> openDialog();

	~ProcedureDialogPresenter();

};

