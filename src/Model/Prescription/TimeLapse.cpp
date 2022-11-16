#include "TimeLapse.h"
#include <array>

struct DurationStruct { std::string_view key, singular, plural; };

constexpr int durations_size = 7;

constexpr std::array<DurationStruct, durations_size> mapping
{ {
	{"d", "ден",  "дни"},
	{"a", "година", "години"},
	{"h", "час", "часа"},
	{"min", "минута", "минути"},
	{"mo", "месец", "месеца"},
	{"s", "секунда", "секунди"},
	{"wk", "седмица", "седмици"}
} };

std::vector<std::string> TimeLapse::getUnitNamesList() const
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

std::string TimeLapse::getUnitName() const {

	if (value == 1) return mapping[m_unit].singular.data();

	return mapping[m_unit].plural.data();
}

std::string TimeLapse::getNHISKey() const
{
	return mapping[m_unit].key.data();
}

bool TimeLapse::setUnit(int unitKey)
{
	if (unitKey < 0 || unitKey >= durations_size) return false;

	m_unit = unitKey;
	return true;
}
