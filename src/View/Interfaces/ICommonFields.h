#pragma once
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractSpinBox.h"
#include "View/Interfaces/AbstractDateEdit.h"
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/AbstractSurfaceSelector.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class AbstractSubPresenter;

class ICommonFields
{
protected:
	AbstractSubPresenter* presenter{nullptr};
public:

	enum BridgeCheckState { Checked, Unchecked, Hidden };

	enum WidgetLayout { General, Restoration, Crown, Range };

	virtual AbstractLineEdit* procedureNameEdit() = 0;
	virtual AbstractLineEdit* diagnosisEdit() = 0;
	//virtual AbstractSpinBox* priceEdit() = 0;
	virtual AbstractDateEdit* dateEdit() = 0;
	//if code is empty, the checkbox will be disabled
	virtual void setKSMPCode(const std::string& code) = 0;
	virtual std::string getKSMPCode() = 0;
	virtual std::string getNotes() = 0;
	virtual void setCurrentPresenter(AbstractSubPresenter* presenter) { this->presenter = presenter; }
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual AbstractSurfaceSelector* surfaceSelector() = 0;
	virtual void setBridgeCheckState(BridgeCheckState state) = 0;
	virtual void disableBridgeCheck(bool disabled) = 0;
	virtual void setLayout(WidgetLayout layout, bool showHyperdontic = false) = 0;
	virtual void setErrorMsg(const std::string& error) = 0;
	virtual void setNotes(const std::string& notes) = 0;
	virtual void setNhifLayout(bool nhif) = 0;
	virtual void setFinancingSource(Procedure::FinancingSource s) = 0;
	virtual Procedure::FinancingSource getFinancingSource() = 0;
	virtual void setHyperdonticState(bool checked) = 0;
	virtual bool onHyperdontic() = 0;
};