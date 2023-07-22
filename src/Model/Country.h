#pragma once
#include <string>
#include <unordered_map>

class Country {

	int m_idx{ 0 };

public:

	Country() {};
	Country(const std::string& code);
	Country(int index);

	const std::string& getCode() const;
	const std::string& getName() const;
	int getIndex() const { return m_idx; }

	static std::vector<std::string> getCodeCountryPair();

	static void initialize();
};