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


const std::array<std::string_view, 17>& ObturationData::colorStrings()
{
	static const std::array<std::string_view, 17> restorationColor
	{
		"",
		"A1", "A2", "A3", "A3,5", "A4",
		"B1", "B2", "B3", "B4",
		"C1", "C2", "C3", "C4",
		"D2", "D3", "D4"
	};

	return restorationColor;
}



std::string ObturationData::infoStr() const
{
	std::string result;
	if (material.size()) result += material + " ";

	result += getInfoResult(color, colorStrings(), u8"Цвят: ", "");

	return result;
}

const std::array<std::string_view, 46> crownColor{
	"",
	"A1", "A2", "A3", "A3,5", "A4",
	"B1", "B2", "B3", "B4",
	"C1", "C2", "C3", "C4",
	"D2", "D3", "D4",
	"0M1", "0M2", "0M3",
	"1M2", "1M3",
	"2L1,5", "2L2,5", "2M1", "2M2", "2M3", "2R1,5", "2R2,5",
	"2L1,5", "2L2,5", "2M1", "2M2", "2M3", "2R1,5", "2R2,5",
	"2L1,5", "2L2,5", "2M1", "2M2", "2M3", "2R1,5", "2R2,5",
	"5M1", "5M2", "5M3"
};

std::string CrownData::infoStr()
{
	std::string result;

	result += getInfoResult(prep_type, prepTypes(), "", "<br>");

	if (material.size()) result += "Материал: " + material + "<br>";
	
	result += getInfoResult(color, crownColor, u8"Цвят: ", "<br> ");

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
