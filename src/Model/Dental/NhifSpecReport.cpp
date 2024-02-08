#include "NhifSpecReport.h"

#include <map>
#include <algorithm>

#include "NhifProcedures.h"
#include "Model/FreeFunctions.h"
#include "Model/UserStructs.h"

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

NhifSpecReport::NhifSpecReport(const Doctor& d, Date reportDate, NhifSpecificationType specType) :
	doctor_name(d.getFullName(true)),
	m_specialty(d.specialty),
	dateFrom(reportDate),
	dateTo(reportDate.getMaxDateOfMonth()),
	m_specificationType(specType)

{

	for (auto code : NhifProcedures::getNhifProcedures(reportDate, m_specialty, false, true, specType))
	{
		procedures_minor[code.oldCode()] = 0;
	}

	for (auto code : NhifProcedures::getNhifProcedures(reportDate, m_specialty, true, true, specType))
	{
		procedures_adult[code.oldCode()] = 0;
	}


}

void NhifSpecReport::addProcedure(const Procedure& p, bool adult, NhifSpecificationType spec)
{
	if (spec != m_specificationType) return;

	if (!p.isNhif()) return;

	auto& map = adult ? procedures_adult : procedures_minor;

	map[p.code.oldCode()] += 1;

}

std::string NhifSpecReport::getSpecString() const
{
	std::string result = m_specialty == NhifSpecialty::General ?
		"първична" : "специализирана"
		;

	result += " дентална помощ";

	const char* specStr[3] = {
		" изцяло или частично заплащана от НЗОК",
		" изцяло заплащана от НЗОК",
		" за лица с психични заболявания под обща анестезия"
	};

	result += specStr[static_cast<int>(m_specificationType)];

	return result;
}

PlainTable NhifSpecReport::getSpecificationReport() const
{
	PlainTable result;

	result.addColumn({ "Код",100,PlainColumn::Center });
	result.addColumn({ "Дейност",300,PlainColumn::Left });
	result.addColumn({ "Брой",120,PlainColumn::Center });
	result.addColumn({ "Ед. цена",120,PlainColumn::Center });
	result.addColumn({ "Обща сума",120,PlainColumn::Center });

	auto key = PriceKey{
		.specialty = (int)m_specialty,
		.adult = false,
		.specification = 0,
		.procedure_code = 0
	};

	//creating the table for minor procedures:

	double sumMinor = 0;

	result.addEmptyRow();
	result.data[1].rows.back().data = "Дентални дейности за лица под 18 години:";

	for (auto& [code, count] : procedures_minor) {

		key.adult = false;
		key.procedure_code = code;

		double price = NhifProcedures::getPriceValue(key, dateFrom).nhif_price;
		double total = count * price;

		sumMinor += total;

		result.addCell(0, { std::to_string(code) });
		result.addCell(1, { nhif_names.at(code) });
		result.addCell(2, { std::to_string(count) });
		result.addCell(3, { FreeFn::formatDouble(price) });
		result.addCell(4, { FreeFn::formatDouble(total) });

	}

	result.addEmptyRow();
	result.data[1].rows.back().data = "Общо за дейности за лица под 18 години::";
	result.data[4].rows.back().data = FreeFn::formatDouble(sumMinor);

	result.addEmptyRow();
	result.data[0].rows.back().data = " ";

	//creating the table for the adult procedures:

	double sumMajor = 0;

	result.addEmptyRow();
	result.data[1].rows.back().data = "Дентални дейности за лица над 18 години:";

	for (auto& [code, count] : procedures_adult) {

		key.adult = true;
		key.procedure_code = code;

		double price = NhifProcedures::getPriceValue(key, dateFrom).nhif_price;
		double total = count * price;

		sumMajor += total;

		result.addCell(0, { std::to_string(code) });
		result.addCell(1, { nhif_names.at(code) });
		result.addCell(2, { std::to_string(count) });
		result.addCell(3, { FreeFn::formatDouble(price) });
		result.addCell(4, { FreeFn::formatDouble(total) });

	}

	result.addEmptyRow();
	result.data[1].rows.back().data = "Общо за дейности за лица над 18 години:";
	result.data[4].rows.back().data = FreeFn::formatDouble(sumMajor);

	result.addEmptyRow();
	result.data[0].rows.back().data = " ";

	result.addEmptyRow();
	result.data[1].rows.back().data = "Общо всички дейности: ";
	result.data[4].rows.back().data = FreeFn::formatDouble(sumMinor + sumMajor);

	return result;
}
