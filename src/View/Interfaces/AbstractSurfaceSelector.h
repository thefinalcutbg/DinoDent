#pragma once
#include "AbstractUIElement.h"
#include "Model/Dental/Procedure.h"

class AbstractSurfaceSelector : public AbstractUIElement
{

	virtual bool validationOperation() override
	{
		return validator->validateInput(getData().surfaces);
	}

public:

	virtual ProcedureObtData getData() = 0;
	virtual void setData(const ProcedureObtData& data) = 0;
	virtual void set_hidden(bool hidden) = 0;
};

