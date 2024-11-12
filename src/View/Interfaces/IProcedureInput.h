#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSpinBox.h"
#include "View/Interfaces/AbstractDateEdit.h"
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "View/Interfaces/AbstractRangeEdit.h"
#include "View/Interfaces/AbstractComboBox.h"
#include <optional>
#include <variant>

class ProcedureCreator;

class IProcedureInput
{
protected:
	ProcedureCreator* presenter{nullptr};
public:

	struct CommonData {

		Diagnosis diagnosis;
		FinancingSource financingSource{ FinancingSource::None };
		std::string notes;
		double value;
	};

	struct ResultData : public CommonData {
		std::variant<
			std::monostate, //no parameters
			int, //anesthesia minutes
			bool, //supernumeral for tooth specific
			std::pair<bool, RestorationData>, //restoration
			ConstructionRange //range
		> parameters;
	};

	virtual AbstractDateEdit* dateEdit() = 0;

	virtual void setCommonData(const CommonData& data, bool hasNhifCode) = 0;

	virtual void setParameterData() = 0; //general procedures
	virtual void setParameterData(AnesthesiaMinutes minutes) = 0; //anesthesia
	virtual void setParameterData(bool supernumeral) = 0; //teeth specific procedures
	virtual void setParameterData(bool supernumeral, RestorationData r) = 0; //restorations
	virtual void setParameterData(ConstructionRange range, bool allowSingle) = 0; //range based procedures
	virtual void setParameterData(bool supernumeral, ConstructionRange range, bool preferSingle) = 0; //crown with block crown option
	virtual void setParameterData(bool supernumeral, ConstructionRange range, RestorationData r, int preferedIndex) = 0; //fuck achi :@ :@ :@

	virtual ResultData getResult() = 0;

	//returns string if an error is found
	virtual std::string isValid() = 0;

	virtual void setErrorMsg(const std::string& errorMsg) = 0;

	//in case of editing already created procedure, the post should be disabled since its already a separate procedure
	virtual void disablePost() = 0;

	virtual void setCurrentPresenter(ProcedureCreator* presenter) { this->presenter = presenter; }

};