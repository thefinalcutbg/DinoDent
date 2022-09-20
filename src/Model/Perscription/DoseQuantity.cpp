#include "DoseQuantity.h"

#include <map>
#include <JsonCpp/json.h>
#include "Resources.h"

#include <qstring.h>

const std::pair<std::string_view, std::string_view> s_defaultDoseParse{ 
			u8"приложение", u8"приложения" 
};

const std::map<int, std::pair<std::string_view, std::string_view>> s_doseParse{
	{3,		{u8"капсула", u8"капсули"}},
	{4,		{u8"капсула", u8"капсули"}},
	{15,	{u8"пръчица", u8"пръчици"}},
	{74,	{u8"пръчица", u8"пръчици"}},
	{19,	{u8"капка", u8"капки"}},
	{27,	{u8"дъвка", u8"дъвки"}},
	{44,	{u8"дъвка", u8"дъвки"}},
	{35,	{u8"международна единица", u8"международни единици"}},
	{36,	{u8"промивка", u8"промивки"}},
	{41,	{u8"таблетка за смучене", u8"таблетки за смучене"}},
	{70,	{u8"разтворима таблетка", u8"разтворими таблетки"}},
	{78,	{u8"таблетка", u8"таблетки"}},
	{79,	{u8"тампон", u8"тампона"}},
	{81,	{u8"промивка", u8"промивки"}},
	{82,	{u8"тест лента", u8"тест ленти"}}
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
