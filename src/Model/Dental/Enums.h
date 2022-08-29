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


constexpr std::string_view statusNames[statusCount]
{
	u8"Временен зъб", u8"Обтурация", u8"Кариес",  u8"Пулпит", u8"Периодонтит",
	u8"Ендодонтско лечение", u8"Радикуларен щифт", u8"Корен",u8"Фрактура", u8"Екстракция",
	u8"Пародонтит", u8"Първа степен", u8"Втора степен", u8"Трета степен",
	u8"Корона", u8"Мост/Блок корони", u8"Фибровлакно", u8"Имплант", u8"Свръхброен зъб", u8"Ретениран зъб"
};