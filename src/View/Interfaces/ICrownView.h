#pragma once

#include <optional>
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/AbstractRangeEdit.h"

class ProcedureCreator;

class ICrownView
{
public:
	virtual AbstractRangeEdit* rangeWidget() = 0;
	virtual void set_hidden(bool hidden) = 0;
	//sets the bridge checkbox to true
	virtual void setData(const std::optional<ConstructionRange>& data) = 0;
	virtual std::optional<ConstructionRange> getData() = 0;
	//sets the bridge checkbox to false
	virtual void lockBridgeCheckbox(bool asChecked = true) = 0;
	virtual void setPresenter(ProcedureCreator* p) = 0;
};