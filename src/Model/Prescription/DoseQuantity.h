#pragma once
#include <vector>
#include <string>
#include <variant>

class DoseQuantity
{  
	int m_unit{ 0 };
	std::string m_ucum;
	bool is_ucum{ false };

public:

	static void initialize();

	bool setUnitFromCL010(const std::string& formStr);
	bool setUnit(int unit);
	void setUnit(const std::string& unitName);

	double value{ 1 };

	std::string getUnitName() const;

	std::string getParsedUnit() const;

	bool isValid() const;

	std::string getXmlUnitValue() const;

	const std::vector<std::string>& unitNames() const;

	std::variant<int, std::string> getUnit() const;

};