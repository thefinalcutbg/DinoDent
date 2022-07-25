#pragma once
#include <vector>
#include <string>
#include <variant>
#include <string_view>
#include <array>
struct Vita3dMaster
{
	const int m_lightness;
	const int m_chroma;
};

class VitaColor
{
	int m_index{0};
public:

	inline static constexpr int vitaClassicCount = 17;

	inline static constexpr std::array<std::string_view, 6> lightness{"0", "1", "2", "3", "4", "5" };

	inline static constexpr std::array<std::string_view, vitaClassicCount> vitaClassic
	{
		"",
		"A1", "A2", "A3", "A3,5", "A4",
		"B1", "B2", "B3", "B4",
		"C1", "C2", "C3", "C4",
		"D2", "D3", "D4"
	};

	
	VitaColor(int vitaClassicIndex = 0);
	VitaColor(const Vita3dMaster& vita3d);
	VitaColor(int vita3dLightness, int vita3dChroma);
	inline int getIndex() const { return m_index; };
	
	static std::vector<std::string> getChroma(int lightness);

	std::variant<int, Vita3dMaster> toColor() const;

	const std::string_view& getColorString() const;

};

