#include "VitaColor.h"

//lightness 0 and 5;
constexpr std::array<std::string_view, 3> idx0_5{"M1", "M2", "M3" }; 

//lightness 1
constexpr std::array<std::string_view, 2> idx1{ "M2", "M3"};     

//ligthness 2, 3, 4
constexpr std::array<std::string_view, 7> idx2_3_4{ "L1,5", "L2,5", "M1", "M2", "M3", "R1,5", "R2,5" };

constexpr std::array<std::string_view, 46> colorStrings{
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

std::vector<std::string> VitaColor::getChroma(int lightness)
{

    std::vector<std::string>result;

    switch (lightness)
    {
        case 0: case 5:
        {
            for (const auto& s : idx0_5) result.push_back(s.data());
            break;
        }

        case 1:
        {
            for (const auto& s : idx1) result.push_back(s.data());
            break;
        }

        case 2: case 3: case 4:
        {
            for (const auto& s : idx2_3_4) result.push_back(s.data());
            break;
        }
    }

    return result;

}

constexpr std::array<Vita3dMaster, 29> vita3dMap
{{
    {0,0},{0,1},{0,2},                              //lightness 0
    {1,0},{1,1},                                    //lightness 1
    {2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},      //lightness 2
    {3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},      //lightness 3
    {4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},      //lightness 4
    {5,0},{5,1},{5,2}                               //lightness 5
}};


VitaColor::VitaColor(int index)
{
    if (index < 0 || index >(vitaClassicCount + vita3dMap.size())) return;

     m_index = index;
}



VitaColor::VitaColor(int vita3dLightness, int vita3dChroma) : m_index{ 0 }
{

    for (int i = 0; i < vita3dMap.size(); i++) {

                if (vita3dMap[i].m_lightness != vita3dLightness ||
                    vita3dMap[i].m_chroma != vita3dChroma) 
                {
                    continue;
                }

        m_index = 17 + i;
        return;

    }

    
}

VitaColor::VitaColor(const Vita3dMaster& vita3d)
    : VitaColor(vita3d.m_lightness, vita3d.m_chroma) {}


std::variant<int, Vita3dMaster> VitaColor::toColor() const
{
    if (m_index < 17) return m_index;

    auto vita3dIdx = m_index - 17;

    return vita3dMap[vita3dIdx];

}

const std::string_view& VitaColor::getColorString() const
{
    return colorStrings[m_index];
}