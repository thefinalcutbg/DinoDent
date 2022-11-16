#include "Dosage.h"
#include "Model/FreeFunctions.h"

std::string Dosage::parse() const
{

	auto removeTrailing = [](double a) {
		auto result = std::to_string(a);

		while (result.back() == '0' && result.size()) result.pop_back();

		if (result.back() == '.') result.pop_back();

		return result;
	};

	std::string result;
	result.reserve(200);

	result += removeTrailing(doseQuantity.value) + " ";

	result += doseQuantity.getParsedUnit() + " ";
	 
	if (route.getKey() != 0) result += route.getValueStr() + " ";

	//frequency parse
	if (frequency == 1) {
		result += "веднъж на ";
	}
	else {
		result += "по " + std::to_string(frequency) + " пъти на ";
	}

	//period parse
	if(period.value == 1){
		
		result += period.getUnitName() + " ";
	}
	else {

		result += "всеки " +
				  removeTrailing(period.value) + 
				  " " + 
				  period.getUnitName() + 
				  " ";
	}

	//when parse
	result += when.parse();

	//bounds parse
	if (bounds.value != 0)
	{
		result += "за " +
		          removeTrailing(bounds.value) + 
				  " " +
				  bounds.getUnitName() + 
				  " ";
	}

	result += additionalInstructions;

	return result;

}