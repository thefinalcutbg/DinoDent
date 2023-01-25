#pragma once

#include <string_view>

enum class ToothType { Molar, Premolar, Frontal };
enum class Quadrant { First, Second, Third, Fourth };
enum class BridgePos { Begin, Middle, End };
enum class Degree { First, Second, Third };


namespace StatusCode
{
	enum StatusCode { Temporary, Obturation, Caries, Pulpitis, ApicalLesion, EndoTreatment, Post, Root, Fracture, Extraction, Periodontitis, Mobility1, Mobility2, Mobility3, Crown, Bridge, FiberSplint, Implant, Dsn, Impacted};
}

namespace Surface
{
	enum Surface { Occlusal, Medial, Distal, Buccal, Lingual, Cervical };
}

constexpr int statusCount = 20;
constexpr int surfaceCount = 6;
constexpr int mobilityCount = 3;

constexpr std::string_view statusNames[statusCount]
{
	"Временен зъб", "Обтурация", "Кариес",  "Пулпит", "Периодонтит",
	"Ендодонтско лечение", "Радикуларен щифт", "Корен","Фрактура", "Екстракция",
	"Пародонтит", "Първа степен", "Втора степен", "Трета степен",
	"Корона", "Мост/Блок корони", "Фибровлакно", "Имплант", "Свръхброен зъб", "Ретениран зъб"
};