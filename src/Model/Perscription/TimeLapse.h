#pragma once
#include <string>
#include <vector>

class TimeLapse {

public:

	enum Unit { Day, Year, Hour, Minute, Month, Second, Week };

	double value{ 1 };

	//returns list of duration names according to the value(singular/plural)
	std::vector<std::string> getUnitNamesList();

	std::string getUnitName();
	std::string getNHISKey();

	//returns true on success
	bool setUnit(Unit u);
	bool setUnit(int u) { return setUnit(static_cast<Unit>(u)); }

private:
	int m_unit{ Unit::Day };

};