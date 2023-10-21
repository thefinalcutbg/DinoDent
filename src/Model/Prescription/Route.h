#pragma once
#include <string>
#include <vector>

class Route {

	int m_key{ 0 }; //default
public:

	Route() {};
	Route(int key);

	bool set(int key);
	bool set(const std::string& valueStr);
	//if key is 0, no parsing needed
	int getKey() const { return m_key; };
	const std::string& getValueStr() const;

	static void initialize();
	static bool isValidStr(const std::string& valueStr);
	static const std::vector<std::string>& getStringList();
};

