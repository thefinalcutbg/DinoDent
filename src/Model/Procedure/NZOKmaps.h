#pragma once
#include <tuple>
#include <unordered_map>
#include "Model/Date.h"
#include <QDebug>

typedef double nzokPrice;
typedef double patientPrice;

struct PriceKey
{
	int specialty;
	bool adult;
	bool unfav;

	bool operator == (const PriceKey& p) const 
	{
		return (specialty == p.specialty && 
				adult == p.adult &&
				unfav == p.unfav);
	}
};

template <>
struct std::hash<PriceKey>
{
	std::size_t operator() (const PriceKey& node) const
	{
		std::size_t h1 = std::hash<int>()(node.specialty);
		std::size_t h2 = std::hash<bool>()(node.adult);
		std::size_t h3 = std::hash<bool>()(node.unfav);

		return h1 ^ h2 ^ h3;
	}
};

struct PriceObj
{
	int bigCode;
	std::unordered_map<int, std::pair<patientPrice, nzokPrice> > priceMap;
};

#include <unordered_set>

struct ProcedurePackage
{
	int timeframe;
	std::unordered_set<int> codes;
	int limit_adult{ -1 };
	int limit_minor{ -1 };
	int limit_pregnant{ -1 };
};


struct NZOKUpdates
{
	Date date;
	std::unordered_map<PriceKey, PriceObj> prices;
	std::vector<ProcedurePackage> packages;
};





