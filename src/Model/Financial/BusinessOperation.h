#pragma once
#include <string>
#include <vector>

struct BusinessOperation
{
	int activity_code;
	std::string activity_name;
	double unit_price;
	int quantity;
	double value_price;
};

typedef std::vector<BusinessOperation> BusinessOperations;