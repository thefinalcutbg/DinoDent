#pragma once
#include <vector>
#include <string>

class DoseQuantity
{  
	int m_unit{ 0 };
public:

	static void initialize();
	static bool isValidFormName(const std::string& formName);

	bool setUnitFromCL010(const std::string& formStr);
	bool setUnit(int unit);
	bool setUnit(const std::string& unitName);

	double value{ 1 };

	std::string getUnitName();

	std::string getParsedUnit();

	const std::vector<std::string>& unitNames();

};