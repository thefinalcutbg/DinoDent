#pragma once
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Model/Date.h"


typedef double nzokPrice;
typedef double patientPrice;

/*
The presense and the price of each procedure depends on 4 variables:
1. The specialty of the doctor
2. Is the patient adult or not
3. The type of specification (partially, fully funded or general anesthesia for mentally ill patients)
4. The nhif code ot the procedure
*/
struct PriceKey
{
	int specialty;
	bool adult;
	int specification;
	int procedure_code;

	bool operator == (const PriceKey& p) const 
	{
		return (specialty == p.specialty && 
				adult == p.adult &&
				specification == p.specification &&
				procedure_code == p.procedure_code);
	}
};

template <>
struct std::hash<PriceKey>
{
	std::size_t operator() (const PriceKey& node) const
	{
		std::size_t h1 = std::hash<int>()(node.specialty);
		std::size_t h2 = std::hash<bool>()(node.adult);
		std::size_t h3 = std::hash<int>()(node.specification);
		std::size_t h4 = std::hash<int>()(node.procedure_code);
		return h1 ^ h2 ^ h3 ^ h4;
	}
};

struct PriceValue
{
	int big_code;
	double nhif_price;
	double patient_price;

};

struct ProcedurePackage
{
	int timeframe;
	std::unordered_set<int> codes;
	int limit_adult{ -1 };
	int limit_minor{ -1 };
};


struct NRD
{
	Date date;
	std::unordered_map<PriceKey, PriceValue> prices;
	std::vector<ProcedurePackage> packages;
};





