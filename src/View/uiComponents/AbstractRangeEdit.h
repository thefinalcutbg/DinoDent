#pragma once
#include "AbstractUIElement.h"
#include <tuple>

class AbstractRangeEdit : public AbstractUIElement
{

protected:
	bool validationOperation() {
		
		auto [begin, end] = getRange();
	
		return validator->validateInput(begin, end);

	};
	
public:
	virtual void setRange(int begin, int end) = 0;
	virtual std::tuple<int, int> getRange() = 0;
	virtual void disbleBridgeSwitch(bool disable) = 0;
};

