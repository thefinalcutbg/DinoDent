#pragma once
#include "AbstractUIElement.h"

class AbstractSurfaceSelector : public AbstractUIElement
{

	virtual bool validationOperation() override
	{
		return validator->validateInput(getSurfaces());
	}

public:
	virtual std::array<bool, 6> getSurfaces() = 0;
	virtual void setSurfaces(const std::array<bool, 6>& surfaces) = 0;
};

