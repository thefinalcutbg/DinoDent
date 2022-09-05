#include "DoseQuantity.h"

#include <map>
#include <JsonCpp/json.h>
#include "Resources.h"

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

bool DoseQuantity::isValidFormName(const std::string& formName)
{
	for (int i = 1; i < s_keyToDoseUnitSingular.size(); i++)
	{
		if (s_keyToDoseUnitSingular[i] == formName) return true;
		if (s_keyToDoseUnitPlural[i] == formName) return true;
	}

	return false;
}

std::string DoseQuantity::getParsedUnit()
{
	std::string result;

	if (s_doseParse.count(m_unit)) {

		result = value == 1 ? s_doseParse.at(m_unit).first.data() : s_doseParse.at(m_unit).second.data();
	}
	else{

		result = value == 1 ? s_defaultDoseParse.first.data() : s_defaultDoseParse.second.data();
	}

	result.append(u8" по ");

	return result;
}

std::string DoseQuantity::getUnitName() {

	if (value == 1) return s_keyToDoseUnitSingular[m_unit];

	return s_keyToDoseUnitPlural[m_unit];
}

const std::vector<std::string>& DoseQuantity::unitNames()
{
	if (value == 1) return s_keyToDoseUnitSingular;

	return s_keyToDoseUnitPlural;
}

#include <qstring.h>

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
		return true;
	}

	return false;
}

bool DoseQuantity::setUnit(const std::string& unitName)
{
	for (int i = 1; i < s_keyToDoseUnitSingular.size(); i++)
	{
		if (unitName == s_keyToDoseUnitSingular[i])
		{
			m_unit = i;
			return true;
		}

		if (unitName == s_keyToDoseUnitPlural[i])
		{
			m_unit = i;
			return true;
		}
	}

	return false;

}
