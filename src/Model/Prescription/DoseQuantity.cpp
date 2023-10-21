#include "DoseQuantity.h"

#include <map>
#include <JsonCpp/json.h>
#include "Resources.h"

#include <qstring.h>

const std::pair<std::string_view, std::string_view> s_defaultDoseParse{ 
			"приложение", "приложения" 
};

const std::map<int, std::pair<std::string_view, std::string_view>> s_doseParse{
	{3,		{"капсула", "капсули"}},
	{4,		{"капсула", "капсули"}},
	{15,	{"пръчица", "пръчици"}},
	{74,	{"пръчица", "пръчици"}},
	{19,	{"капка", "капки"}},
	{27,	{"дъвка", "дъвки"}},
	{44,	{"дъвка", "дъвки"}},
	{35,	{"международна единица", "международни единици"}},
	{36,	{"промивка", "промивки"}},
	{41,	{"таблетка за смучене", "таблетки за смучене"}},
	{70,	{"разтворима таблетка", "разтворими таблетки"}},
	{78,	{"таблетка", "таблетки"}},
	{79,	{"тампон", "тампона"}},
	{81,	{"промивка", "промивки"}},
	{82,	{"тест лента", "тест ленти"}}
};

std::vector<std::string> s_keyToDoseUnitSingular;
std::vector<std::string> s_keyToDoseUnitPlural;

void DoseQuantity::initialize()
{
	Json::Value doseUnits;

	Json::Reader().parse(Resources::fromPath(":/json/json_dose.json"), doseUnits);

	auto size = doseUnits.size();

	//the first element is null
	s_keyToDoseUnitSingular = { size + 1, std::string{} };
	s_keyToDoseUnitPlural = { size + 1, std::string{} };

	for (auto& unit : doseUnits)
	{
		auto key = unit["Key"].asInt();

		s_keyToDoseUnitSingular[key] = unit["Singular"].asString();
		s_keyToDoseUnitPlural[key] = unit["Plural"].asString();
	}
}


DoseQuantity::DoseQuantity(int unit) : m_unit(unit)
{
	if (isValid()) return;

	m_unit = 0;
}

DoseQuantity::DoseQuantity(const std::string& ucum)
{
	m_ucum = ucum;
}


std::string DoseQuantity::getParsedUnit() const
{
	if (is_ucum) return m_ucum + " по ";

	std::string result;

	if (s_doseParse.count(m_unit)) {

		result = value == 1 ? s_doseParse.at(m_unit).first.data() : s_doseParse.at(m_unit).second.data();
	}
	else{

		result = value == 1 ? s_defaultDoseParse.first.data() : s_defaultDoseParse.second.data();
	}

	return result;
}

std::string DoseQuantity::getUnitName() const {

	if (is_ucum) return m_ucum;

	if (value == 1) return s_keyToDoseUnitSingular[m_unit];

	return s_keyToDoseUnitPlural[m_unit];
}

const std::vector<std::string>& DoseQuantity::unitNames() const
{
	if (value == 1) return s_keyToDoseUnitSingular;

	return s_keyToDoseUnitPlural;
}

std::variant<int, std::string> DoseQuantity::getUnit() const
{
	if (is_ucum) return m_ucum;

	return m_unit;
}

bool DoseQuantity::isValid() const
{
	//insert UCUM mapping?
	if (is_ucum) return m_ucum.size();

	return m_unit >= 0 && m_unit < s_keyToDoseUnitSingular.size();
}

std::string DoseQuantity::getXmlUnitValue() const
{
	if (is_ucum) return m_ucum;

	if (!m_unit) return {};

	return std::to_string(m_unit);
}

bool DoseQuantity::setUnitFromCL010(const std::string& formStr)
{
	int new_unit = -1;

	QString form_cl010 = QString::fromStdString(formStr).toLower();

	for (int i = 1; i < s_keyToDoseUnitSingular.size(); i++)
	{
		QString formSimple = QString::fromStdString(s_keyToDoseUnitSingular[i]).toLower();

		if (form_cl010.contains(formSimple))
		{
			new_unit = i;
		}

	}

	if(new_unit == -1) return false;

	m_unit = new_unit; return true;
}

bool DoseQuantity::setUnit(int unit)
{
	if (unit > 0 && unit < s_keyToDoseUnitSingular.size()) {
		m_unit = unit;
		is_ucum = false;
		return true;
	}

	return false;
}

void DoseQuantity::setUnit(const std::string& unitName)
{
	for (int i = 1; i < s_keyToDoseUnitSingular.size(); i++)
	{
		if (unitName == s_keyToDoseUnitSingular[i])
		{
			m_unit = i;
			is_ucum = false;
			return;
		}

		if (unitName == s_keyToDoseUnitPlural[i])
		{
			m_unit = i;
			is_ucum = false;
			return;
		}
	}

	is_ucum = true;
	m_ucum = unitName;

}
