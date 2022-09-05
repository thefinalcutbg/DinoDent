#include "TimeLapse.h"
#include <array>

struct DurationStruct { std::string_view key, singular, plural; };

constexpr int durations_size = 7;

constexpr std::array<DurationStruct, durations_size> mapping
{ {
	{"d", u8"ден",  u8"дни"},
	{"a", u8"година", u8"години"},
	{"h", u8"час", u8"часа"},
	{"min", u8"минута", u8"минути"},
	{"mo", u8"месец", u8"месеца"},
	{"s", u8"секунда", u8"секунди"},
	{"wk", u8"седмица", u8"седмици"}
} };

std::vector<std::string> TimeLapse::getUnitNamesList()
{
	std::vector<std::string> result;
	result.reserve(durations_size);

	if (value == 1)
	{
		for (auto& obj : mapping) result.push_back(obj.singular.data());
	}
	else
	{
		for (auto& obj : mapping) result.push_back(obj.plural.data());
	}

	return result;
}

std::string TimeLapse::getUnitName() {

	if (value == 1) return mapping[m_unit].singular.data();

	return mapping[m_unit].plural.data();
}

std::string TimeLapse::getNHISKey()
{
	return mapping[m_unit].key.data();
}

bool TimeLapse::setUnit(Unit u)
{
	if (u < 0 || u >= durations_size) return false;

	m_unit = u;
	return true;
}
