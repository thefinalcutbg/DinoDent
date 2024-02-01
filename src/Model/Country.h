#pragma once
#include <string>
#include <vector>
class Country {

    std::size_t m_idx{ 0 };

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
