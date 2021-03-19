#pragma once
#include "AbstractUIElement.h"
#include <tuple>

class AbstractRangeEdit : public AbstractUIElement
{

protected:
	bool validationOperation() {
		
		auto range = getRange();

		return validator->validate(std::get<0>(range), std::get<1>(range));
	};
	
public:
	virtual void setRange(int begin, int end) = 0;
	virtual std::tuple<int, int> getRange() = 0;
};

