#include "StatusData.h"


std::string ObturationData::infoStr() const
{
	std::string result = material.getName();

	if (result.size()) { result += " "; }

	result.append(std::string(color.getColorString()));

	return result;
}


std::string CrownData::infoStr() const
{
	std::string result;

	result += prep.getName() + "<br>";
	result += "Материал: " + material.getName() + "<br>";

	if (color.getIndex()) {
		result += "Цвят: " + std::string(color.getColorString()) + "<br> ";
	}

	return result;
}



std::string ImplantData::infoStr() const
{
	std::string result;
	if (width) result += "Ширина: " + std::to_string(width) + " mm<br>";
	if (length) result += "Дължина: " + std::to_string(length) + " mm<br>";

	result += type.getName() + "<br>";
	result += time.getName() + "Време на имплантиране: " + "<br>";

	if (tissue_aug.getIndex())
	{
		result += "Тъканна аугментация: " + tissue_aug.getName() + "<br>";
	}

	if (bone_aug.getIndex())
	{
		result += "Костозаместител: " + bone_aug.getName() + " ";
	}

	if (membrane) result += "Мембрана; ";
	if (sinusLift) result += "Синус лифт;";

	return result;
		
}
