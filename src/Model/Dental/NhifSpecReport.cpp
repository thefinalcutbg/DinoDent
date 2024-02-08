#include "NhifSpecReport.h"

#include <map>
#include <algorithm>

#include "NhifProcedures.h"
#include "Model/FreeFunctions.h"

const std::map<int, std::string> nhif_names
{
	{ 101, { "Обстоен преглед със снемане на орален статус" } },
	{ 103, { "Обстоен преглед със снемане на орален статус за бременни" } },
	{ 301, { "Обтурация с амалгама или химичен композит" } },
	{ 332, { "Лечение на пулпит или периодонтит на временен зъб" } },
	{ 333, { "Лечение на пулпит или периодонтит на постоянен зъб" } },
	{ 508, { "Екстракция на временен зъб с анестезия" } },
	{ 509, { "Екстракция на постоянен зъб с анестезия" } },
	{ 510, { "Екстракция на дълбоко фрактуриран или дълбоко разрушен зъб с анестезия" } },
	{ 544, { "Контролен преглед след някоя от горните две дейности" } },
	{ 832, { "Дейност по възстановяване функцията на дъвкателния апарат при цялостна обеззъбена горна челюст с горна цяла плакова зъбна протеза за период от 4 години, в т.ч.и контролни прегледи до два месеца след поставяне на протезата" } },
	{ 833, { "Дейност по възстановяване функцията на дъвкателния апарат при цялостна обеззъбена долна челюст с долна цяла плакова зъбна протеза за период от 4 години, в т.ч.и контролни прегледи до два месеца след поставяне на протезата"  } },
	{ 901, { "Обща анестезия в извънболнично лечебно заведение" } },
	{ 902, { "Обща анестезия в болнично лечебно заведение" } },
	{ 903, { "24-часово активно наблюдение при необходимост след общата анестезия" } }
};

void NhifSpecReport::addProcedure(const Procedure& p, bool adult, NhifSpecificationType spec)
{
	if (!p.isNhif()) return;

	auto key = PriceKey{
		.specialty = static_cast<int>(m_specialty),
		.adult = adult,
		.specification = static_cast<int>(spec),
		.procedure_code = p.code.oldCode()

	};

	if (procedure_map.count(key)) {
		procedure_map[key] += 1;
	}
	else {
		procedure_map[key] = 1;
	}
}

PlainTable NhifSpecReport::getSpecificationReport() const
{
	PlainTable result;

	result.addColumn({ "Код",100,PlainColumn::Center });
	result.addColumn({ "Манипулация",300,PlainColumn::Left });
	result.addColumn({ "Ед. цена",120,PlainColumn::Center });
	result.addColumn({ "Брой",120,PlainColumn::Center });
	result.addColumn({ "Крайна цена",120,PlainColumn::Center });

	struct SpecificationRow {
		int code;
		bool adult;
		int spec;
		std::string name;
		double single_price;
		int count;
		double total_price;
	};

	std::vector<SpecificationRow> rows;

	for (auto& [key, procedureCount] : procedure_map)
	{

		auto value = NhifProcedures::getPriceValue(key, m_reportDate);

		//using vector, since it has to be sorted for the final result

		std::string name = nhif_names.at(key.procedure_code);
		name += key.adult ? " за лица над 18 години" : " за лица до 18 години";

		switch (key.specification) {

			case 1: //full coverage
				name += " (изцяло заплащана от НЗОК)"; break;
			case 2: //general anesthesia
				name += " (обща анестезия)"; break;
			default: break;
		}

		rows.push_back(
			SpecificationRow{
				.code = key.procedure_code,
				.adult = key.adult,
				.spec = key.specification,
				.name = name,
				.single_price = value.nhif_price,
				.count = procedureCount,
				.total_price = value.nhif_price * procedureCount
		});

	}

	std::ranges::sort(rows, [](const SpecificationRow& a, const SpecificationRow& b)
	{
			if (a.adult < b.adult) return true;
			if (a.adult == b.adult && a.spec < b.spec) return true;
			if (a.adult == b.adult && a.spec == b.spec && a.code < b.code) return true;

			return false;
	});

	for (auto& row : rows)
	{
		result.addCell(0, { std::to_string(row.code) });
		result.addCell(1, { row.name });
		result.addCell(2, { FreeFn::formatDouble(row.single_price) });
		result.addCell(3, { std::to_string(row.count) });
		result.addCell(4, { FreeFn::formatDouble(row.total_price) });
	}


	return result;
}

double NhifSpecReport::getTotalPrice() const
{
	double result = 0;

	for (auto& [key, count] : procedure_map) {
		result += NhifProcedures::getPriceValue(key, m_reportDate).nhif_price * count;
	}

	return result;
}
