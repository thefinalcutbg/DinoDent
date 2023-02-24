#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSpinBox.h"
#include "View/Interfaces/AbstractDateEdit.h"
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class ProcedureCreator;

class ICommonFields
{
protected:
	ProcedureCreator* presenter{nullptr};
public:

	enum BridgeCheckState { Checked, Unchecked, Hidden };

	enum WidgetLayout { General, ToothSpecific, Restoration, Range };

	virtual AbstractLineEdit* diagnosisEdit() = 0;
	virtual AbstractDateEdit* dateEdit() = 0;
	virtual std::string getNotes() = 0;
	virtual void setCurrentPresenter(ProcedureCreator* presenter) { this->presenter = presenter; }
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual void setLayout(WidgetLayout layout) = 0;
	virtual void setErrorMsg(const std::string& error) = 0;
	virtual void setNotes(const std::string& notes) = 0;
	virtual void setNhifLayout(bool nhif) = 0;
	virtual void setFinancingSource(FinancingSource s) = 0;
	virtual FinancingSource getFinancingSource() = 0;
	virtual void setHyperdonticState(bool checked) = 0;
	virtual bool onHyperdontic() = 0;
};