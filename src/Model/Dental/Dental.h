#pragma once

#include <string_view>
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
		"Ендодонтско лечение", "Радикуларен щифт", "Корен","Фрактура", "Екстракция",
		"Пародонтит", "Подвижност",
		"Корона", "Мост/Блок корони", "Фибровлакно", "Имплант", "Свръхброен зъб", "Ретениран зъб", "Протеза", "Зъбен камък"
	};

	constexpr std::string_view mobilityNames[MobilityCount]
	{
		"Подвижност I", "Подвижност II", "Подвижност III"
	};

	constexpr std::string_view surfaceNames[SurfaceCount]
	{
		"Оклузално/Инцизално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално"
	};

}