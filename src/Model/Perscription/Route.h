#pragma once
#include <string>
#include <vector>

class Route {

	int m_key{ 0 }; //default
public:

	bool set(int key);
	bool set(const std::string& valueStr);
	int getKey() { return m_key; };
	const std::string& getValueStr();

	static void initialize();
	static bool isValidStr(const std::string& valueStr);
	static const std::vector<std::string>& getStringList();
};

