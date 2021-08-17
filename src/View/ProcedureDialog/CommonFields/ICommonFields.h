#pragma once
#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractSpinBox.h"
#include "View/uiComponents/AbstractDateEdit.h"

class ICommonFieldsPresenter
{
public:
	virtual void diagnosisTextChanged(std::string text) = 0;
};

class ICommonFields
{
protected:
	ICommonFieldsPresenter* presenter{nullptr};
public:

	virtual AbstractLineEdit* manipulationEdit() = 0;
	virtual AbstractLineEdit* diagnosisEdit() = 0;
	virtual AbstractSpinBox* priceEdit() = 0;
	virtual AbstractDateEdit* dateEdit() = 0;
	virtual void set_hidden(bool hidden) = 0;

	virtual void setCurrentPresenter(ICommonFieldsPresenter* presenter) { this->presenter = presenter; }
};