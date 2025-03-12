#pragma once
#include <string>
#include <unordered_map>

struct EkatteData;

class Ekatte
{
	int ekatteIdx{ 0 };
	bool isCity{ 0 };
	int rhif{ 0 };
	int healthRegion{ 0 };
	const std::string* str{ 0 };

	Ekatte(int idx, const EkatteData& data);

public:
	static void initialize();
	static bool isValidStr(const std::string& cityName);

	Ekatte() {}
	Ekatte(int idx);
	Ekatte(const std::string& cityString);
	Ekatte(int rhif, int healthRegion);
	std::string getRhif() const;
	std::string getHealthRegion() const;
	std::string getString(bool prefix = true) const;
	std::string ekatte() const;
	std::string getAddressWithStreet(const std::string& street) const;
	bool isValid() const;
	bool isUnfav() const;
	int getIdxAsInt() const { return ekatteIdx; }
	const std::string& getRegionCode() const;
	static const std::unordered_map<std::string, int>& cityNameToIdx();
};

