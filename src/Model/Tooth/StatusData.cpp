#include "StatusData.h"

//helper function with range checking
template<size_t T>
inline std::string getInfoResult(
	int i,
	const std::array<std::string_view, T>& arr,
	const std::string& description,
	const std::string& suffix
)
{
	if (!i || i >= T) return "";
	
	return description + arr[i].data() + suffix;
}




std::string ObturationData::infoStr() const
{
	std::string result;
	if (material.size()) { result += material + " "; }

	result.append(std::string(color.getColorString()));

	return result;
}

const std::array<std::string_view, 5>& CrownData::prepTypes()
{
	static const std::array<std::string_view, 5>crownPrep{
	"",
	u8"Безпрагово изпиляване",
	u8"Прагово изпиляване",
	u8"Завинтване върху имплант",
	u8"Циментиране върху имплант"
	};


	return crownPrep;
}


std::string CrownData::infoStr()
{
	std::string result;

	result += getInfoResult(prep_type, prepTypes(), "", "<br>");

	if (material.size()) result += "Материал: " + material + "<br>";

	if (color.getIndex()) {
		result = u8"Цвят: " + std::string(color.getColorString()) + "<br> ";
	}

	return result;
}



const std::array<std::string_view, 4>& ImplantData::typeStr()
{
	static const std::array<std::string_view, 4>implType{
	"",
	u8"Закрито",
	u8"Цикатризационен винт",
	u8"Временна корона"
	};

	return implType;
}

const std::array<std::string_view, 4>& ImplantData::timeStr()
{
	static const std::array<std::string_view, 4> implTime{
			"",
			u8"Отложено",
			u8"Ранно",
			u8"Имедиантно"
	};

	return implTime;
}

const std::array<std::string_view, 4>& ImplantData::tissueAugStr()
{
	static const std::array<std::string_view, 4> implAug{
	"",
	"Roll flap",
	"Free gingival graft",
	"Connective tissue graft"
	};

	return implAug;
}

const std::array<std::string_view, 5>& ImplantData::boneAugStr()
{
	static const std::array<std::string_view, 5>boneAug{
	"",
	u8"Ксенографт",
	u8"Алографт",
	u8"Автографт",
	u8"Бета-3-калциев фосфат"
	};

	return boneAug;
}

std::string ImplantData::infoStr() const
{
	std::string result;

	if (system.size()) { result += u8"Система: " + system + "<br>"; };
	if (width) result += u8"Ширина: " + std::to_string(width) + " mm<br>";
	if (length) result += u8"Дължина: " + std::to_string(length) + " mm<br>";

	result += getInfoResult(type, typeStr(), "", "<br>");
	result += getInfoResult(time, timeStr(), u8"Време на имплантиране: ", ";<br>");
	result += getInfoResult(tissue_aug, tissueAugStr(), u8"Тъканна аугментация: ", ";<br>");
	result += getInfoResult(bone_aug, boneAugStr(), u8"Костозаместител: ", " ");

	if (membrane) result += u8"Мембрана; ";
	if (sinusLift) result += u8"Синус лифт;";

	return result;
		
}
