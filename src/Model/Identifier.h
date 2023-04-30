#pragma once
#include <string>

class Identifier
{
	std::string m_id;
	bool is_egn{ true };

	bool egnIsValid(const std::string& id);
	bool lnchIsValid(const std::string& id);

public:

	enum PersonType {
		None = 0,
		EGN = 1,
		LNCH = 2
	};

	Identifier(const std::string& id);
	Identifier(const std::string& id, PersonType t); //omits validation

	PersonType personType() {

		if (!isValid()) return PersonType::None;

		return is_egn ? PersonType::EGN : PersonType::LNCH;

	};

	bool isValid() {
		return m_id.size();
	}
};