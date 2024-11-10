#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSpinBox.h"
#include "View/Interfaces/AbstractDateEdit.h"
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "View/Interfaces/AbstractRangeEdit.h"
#include "View/Interfaces/AbstractComboBox.h"
#include <optional>

class ProcedureCreator;

class IProcedureInput
{
protected:
	ProcedureCreator* presenter{nullptr};
public:

	struct Data {

		ProcedureCode code;
		Diagnosis diagnosis;
		std::optional<ConstructionRange> range;
		bool hyperdontic{ false };
		AdditionalParameters param;
		FinancingSource financingSource{ FinancingSource::None };
		std::string notes;
		double price;
	};

	virtual AbstractDateEdit* dateEdit() = 0;
	virtual void setData(const Data& data) = 0;
	virtual Data getData() = 0;
	//returns string if an error is found
	virtual std::string isValid() = 0;
	virtual void setErrorMsg(const std::string& errorMsg) = 0;
	//in case of editing already created procedure, the post should be disabled since its already a separate procedure
	virtual void disablePost() = 0;
	virtual void setCurrentPresenter(ProcedureCreator* presenter) { this->presenter = presenter; }

};