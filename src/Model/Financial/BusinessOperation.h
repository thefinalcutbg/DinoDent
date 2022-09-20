#pragma once
#include <string>
#include <vector>

struct Procedure;

struct BusinessOperation
{
	BusinessOperation() {};

	BusinessOperation(const std::string& code, const std::string& name, double price, int quantity);
	
	BusinessOperation(const Procedure& p);

	std::string activity_code;
	std::string activity_name;
	double unit_price{ 0 };
	int quantity{ 0 };
	double value_price{ 0 };

	bool operator == (const BusinessOperation& other) const;

	void addQuantity(int quantity);
};

typedef std::vector<BusinessOperation> BusinessOperations;