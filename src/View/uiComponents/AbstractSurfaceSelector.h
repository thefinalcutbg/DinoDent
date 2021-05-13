#pragma once
#include "AbstractUIElement.h"

class AbstractSurfaceSelector : public AbstractUIElement
{
	// Inherited via AbstractUIElement

	virtual bool validationOperation() override
	{
		return validator->validate(getSurfaces());
	}

public:
	virtual void setFocusAndSelectAll() = 0;
	virtual void disable(bool disable) = 0;
	virtual void setAppearence(bool valid) = 0;
	virtual std::array<bool, 6> getSurfaces() = 0;
	virtual void setSurfaces(const std::array<bool, 6>& surfaces) = 0;
};

