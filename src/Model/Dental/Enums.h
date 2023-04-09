#pragma once

#include <string_view>

enum class ToothType { Molar, Premolar, Frontal };
enum class Quadrant { First, Second, Third, Fourth };
enum class BridgePos { Begin, Middle, End };
enum class Degree { First, Second, Third };

constexpr int statusCount = 21;

namespace StatusCode
{
	enum StatusCode	{ Healthy, Temporary, Obturation,	Caries,	Pulpitis,	ApicalLesion,	EndoTreatment,	Post,	Root,	Fracture,	Extraction,	Periodontitis,	Mobility,	Crown,	Bridge,	FiberSplint,	Implant,	Dsn,	Impacted, Denture, Calculus };
}

namespace Surface
{
	enum Surface { Occlusal, Medial, Distal, Buccal, Lingual, Cervical };
}

constexpr int surfaceCount = 6;
constexpr int mobilityCount = 3;

constexpr std::string_view statusNames[statusCount]
{
	"Интактен зъб", "Временен зъб", "Обтурация", "Кариес",  "Пулпит", "Периодонтит",
	"Ендодонтско лечение", "Радикуларен щифт", "Корен","Фрактура", "Екстракция",
	"Пародонтит", "Подвижност",
	"Корона", "Мост/Блок корони", "Фибровлакно", "Имплант", "Свръхброен зъб", "Ретениран зъб", "Изкуствен зъб", "Зъбен камък"
};

constexpr std::string_view mobilityNames[mobilityCount]
{
	"Подвижност I", "Подвижност II", "Подвижност III"
};