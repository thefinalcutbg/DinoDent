#include "getManipulationTamplates.h"

static std::array<int, 12> codes
{ 101, 301, 508, 509, 332, 333, 102, 520, 510, 544, 832, 833 };

static std::array<std::string, 12> names
{
	"Обстоен преглед със снемане на орален статус - в т.ч еднократен допълнителен преглед за бременни",
	"Обтурация с амалгама или химичен композит",
	"Екстракция на временен зъб с анестезия",
	"Екстракция на постоянен зъб с анестезия",
	"Лечение на пулпит или периодонтит на временен зъб",
	"Лечение на пулпит или периодонтит на постоянен зъб",
	"Специализиран обстоен преглед",
	"Инцизия в съединителнотъканни ложи, включително анестезия",
	"Екстракция на дълбоко фрактуриран или дълбоко разрушен зъб, включително анестезия",
	"Контролен преглед след екстракция или инцизия",
	"Дейност по възстановяване функцията на дъвкателния апарат при цялостна обеззъбена горна челюст с горна цяла плакова зъбна протеза, в т.ч. и контролни прегледи за период от 4 години",
	"Дейност по възстановяване функцията на дъвкателния апарат при цялостна обеззъбена горна челюст с долна цяла плакова зъбна протеза, в т.ч. и контролни прегледи за период от 4 години"
};

static std::array<std::string, 12> default_diag
{
	"Преглед",
	"",
	"",
	"",
	"",
	"",
	"Преглед",
	"Мекотъканен абсцес",
	"",
	"Контролен преглед",
	"Adontia totalis maxillaris",
	"Adontia totalis mandibularis"
};

//general, obturation, extraction, endo, crown, bridge, any

static std::array<ManipulationType, 12> types
{
	ManipulationType::general,
	ManipulationType::obturation,
	ManipulationType::extraction,
	ManipulationType::extraction,
	ManipulationType::endo,
	ManipulationType::endo,
	ManipulationType::general,
	ManipulationType::general,
	ManipulationType::extraction,
	ManipulationType::general,
	ManipulationType::general,
	ManipulationType::general
};

static std::array<int, 12> duration
{
	15, 26, 17, 22, 39, 141, 15, 20, 45, 10, 240, 240
};

struct PricePair
{
	double nzok_price;
	double patient_price;
};

static std::array<PricePair, 12> adult
{
	PricePair{10.2, 1.8},
	PricePair{35.18, 4},
	PricePair{0, 0},
	PricePair{35.18, 4},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{11.6, 1.8},
	PricePair{11.9, 7.5},
	PricePair{31.86, 13.5},
	PricePair{3.8, 0.8},
	PricePair{200, 0},
	PricePair{200, 0}
};


static std::array<PricePair, 12> child
{
	PricePair{10.2, 1.8},
	PricePair{39.18, 0},
	PricePair{15.74, 0},
	PricePair{39.18, 0},
	PricePair{21.92, 4.7},
	PricePair{68.93, 12.3},
	PricePair{11.6, 1.8},
	PricePair{16.4, 3},
	PricePair{41.05, 5},
	PricePair{3.8, 0.8},
	PricePair{200, 0},
	PricePair{200, 0}
};

static std::array<PricePair, 12> unfavourable
{
	PricePair{12, 0},
	PricePair{51.08, 0},
	PricePair{15.74, 0},
	PricePair{39.18, 0},
	PricePair{36.60, 0},
	PricePair{91.21, 0},
	PricePair{12.86, 0},
	PricePair{19.40, 0},
	PricePair{46.46, 0},
	PricePair{4.6, 0},
	PricePair{200, 0},
	PricePair{200, 0}
};


static std::array<PricePair, 12> pediatric
{
	PricePair{11.06, 1.8},
	PricePair{41.28, 0},
	PricePair{17.55, 0},
	PricePair{41.28, 0},
	PricePair{24.58, 0},
	PricePair{77.05, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{200, 0},
	PricePair{200, 0}
};

static std::array<PricePair, 12> pediatric_unfav
{
	PricePair{12.86, 0},
	PricePair{55.13, 0},
	PricePair{18.76, 0},
	PricePair{41.28, 0},
	PricePair{39.26, 0},
	PricePair{99.33, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{0, 0},
	PricePair{200, 0},
	PricePair{200, 0}
};

std::vector<ManipulationTemplate> getNZOK(int age, int doctor_code, bool unfav)
{
	std::vector<ManipulationTemplate> manipulations;
	
	std::vector<int> childInt{ 0, 1, 2, 3, 4, 5, 10, 11};
	std::vector<int> adultInt{ 0, 1, 3, 10, 11};
	std::vector<int> surgeryInt{ 6, 7, 8, 9 };

	std::array<PricePair, 12>* currentPriceList { NULL };
	std::vector<int>* currentManipulations{ NULL };

	if (age < 18)
	{
		currentManipulations = &childInt;

		if (doctor_code == 61)
		{
			if (unfav)
			{
				currentPriceList = &pediatric_unfav;
			}
			else
			{
				currentPriceList = &pediatric;
			}
		}
		else
		{
			if (unfav)
			{
				currentPriceList = &unfavourable;
			}
			else
			{
				currentPriceList = &child;
			}
		}
	}
	else
	{
		currentManipulations = &adultInt;

		if (unfav)
		{
			currentPriceList = &unfavourable;
		}
		else
		{
			currentPriceList = &adult;
		}
	}


	if (doctor_code == 62 || doctor_code == 68)
	{
		for (int m : surgeryInt)
		{
			currentManipulations->push_back(m);
		}
	}

	std::array<ManipulationTemplate, 12> allManipulations;

	for (int i = 0; i < 12; i++)
	{
		allManipulations[i] = ManipulationTemplate
		{
			types[i],
			codes[i],
			names[i],
			currentPriceList->at(i).patient_price,
			default_diag[i],
			true,
			duration[i],
			currentPriceList->at(i).nzok_price,
			""
		};
	}

	std::vector<ManipulationTemplate> finalProduct;

	for (int m : *currentManipulations)
	{
		finalProduct.push_back(allManipulations[m]);
	}

	finalProduct[1].material = "Химиополимер";

	return finalProduct;

}

std::vector<ManipulationTemplate> getCustomManipulations()
{
	std::ifstream ifs("manipulation.json");
	Json::Reader reader;
	Json::Value m;

	reader.parse(ifs, m);

	const Json::Value& manipulation = m["manipulation"];

	std::vector<ManipulationTemplate> manipulationList;
	manipulationList.reserve(manipulation.size());

	for (int i = 0; i < manipulation.size(); i++)
	{
		ManipulationTemplate m;
		m.type = static_cast<ManipulationType>(manipulation[i]["type"].asInt());
		m.code = manipulation[i]["code"].asInt();
		m.name = manipulation[i]["name"].asString();
		m.price = manipulation[i]["price"].asDouble();

		if (!manipulation[i]["default_diag"].isNull())
		{
			m.diagnosis = manipulation[i]["default_diag"].asString();
		}
		if (!manipulation[i]["material"].isNull())
		{
			m.material = manipulation[i]["material"].asString();
		}
		if (!manipulation[i]["duration"].isNull())
		{
			m.duration = manipulation[i]["duration"].asInt();
		}



		manipulationList.emplace_back(m);
	}

	return manipulationList;

}