#pragma once

#include <string_view>
#include <vector>

//Some important enums and string literals
namespace Dental
{
	enum Type { Molar, Premolar, Frontal };

	constexpr int teethCount = 32;

	enum Surface { Occlusal, Medial, Distal, Buccal, Lingual, Cervical, SurfaceCount };
	enum Quadrant { First, Second, Third, Fourth };
	enum BridgePos { Begin, Middle, End };
	enum MobilityDegree { I, II, III, MobilityCount };
	enum class StatusType { General, Restoration, Caries, NonCariesLesion, DefectiveRestoration, Mobility };

	enum Status {
		Healthy,
		Temporary,
		Restoration,
		Caries,
		DefectiveRestoration,
		NonCariesLesion,
		Pulpitis,
		Necrosis,
		Resorption,
		ApicalLesion,
		RootCanal,
		Post,
		Root,
		Fracture,
		Missing,
		Periodontitis,
		Mobility,
		Crown,
		Bridge,
		Splint,
		Implant,
		HasSupernumeral,
		Impacted,
		Denture,
		Calculus,
		StatusCount
	};

	constexpr std::string_view statusNames[StatusCount]
	{
		"Интактен зъб", "Временен зъб", "Възстановяване", "Кариес", "Дефектно възстановяване", "Некариозна лезия",  "Пулпит", "Некроза", "Резорбция", "Периодонтит",
		"Ендодонтско лечение", "Радикуларен щифт", "Корен / Разрушен зъб","Фрактура", "Липсващ зъб",
		"Пародонтит", "Подвижност",
		"Корона", "Мост / Блок корони", "Шиниране / Адхезивен мост", "Имплант", "Свръхброен зъб", "Ретениран зъб", "Протеза", "Зъбен камък"
	};

	constexpr std::string_view mobilityNames[MobilityCount]
	{
		"Подвижност I", "Подвижност II", "Подвижност III"
	};

	constexpr std::string_view surfaceNames[SurfaceCount]
	{
		"Оклузално/Инцизално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално"
	};

	static inline const std::vector<std::pair<std::string, std::string>> status_symbols = {
	{"H",   "Интактен зъб"},
	{"O",   "Възстановяване"},
	{"C",   "Кариес"},
	{"NC",  "Некариозна лезия"},
	{"DR",  "Дефект на възстановяване"},
	{"P",   "Пулпит"},
	{"N",   "Некроза на пулпата"},
	{"G",   "Периодонтит"},
	{"F",   "Фрактура"},
	{"R",   "Корен/Разрушен зъб"},
	{"Res", "Резорбция"},
	{"Rc",  "Девитализиран зъб"},
	{"Rp",  "Радикуларен щифт"},
	{"E",   "Липсващ зъб"},
	{"Pa",  "Пародонтит/Периимплантит"},
	{"M1",  "Подвижност(I-ва степен)"},
	{"M2",  "Подвижност(II-ра степен)"},
	{"M3",  "Подвижност(III-та степен)"},
	{"T",   "Зъбен камък"},
	{"I",   "Имплант"},
	{"K",   "Корона (самостоятелна)"},
	{"Kb",  "Корона (мостокрепител)"},
	{"B",   "Изкуствен зъб от фиксирано протезиране"},
	{"X",   "Изкуствен зъб от подвижно протезиране"},
	{"S",   "Шина/Адхезивен мост"},
	{"Re",  "Ретиниран зъб"},
	{"D",   "Свръхброен зъб" }
	};
}
