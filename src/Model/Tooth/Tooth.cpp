#include "Tooth.h"

std::string Tooth::cariesDefault{"Caries media"};
std::string Tooth::pulpitisDefault{ "Pulpitis chronica ulcerosa" };
std::string Tooth::lesionDefault{"Periodontitis chronica granulomatosa"};
std::string Tooth::fractureDefault{ "Fractura coronae dentis" };
std::string Tooth::rootDefault{ "Radix dentis" };
std::string Tooth::obturationDefault{ "Фотополимер" };
std::string Tooth::prosthoDefault{ "Металокерамика" };

Tooth::Tooth() : 
	obturation{ false },
	caries{ false }, 
	index{ -1 }
{
	for (int i = 0; i < 6; i++) {
		c_surf[i].setDefaultAttribute(cariesDefault);
		o_surf[i].setDefaultAttribute(obturationDefault);
	}
	root.setDefaultAttribute(rootDefault);
	pulpitis.setDefaultAttribute(pulpitisDefault);
	fracture.setDefaultAttribute(fractureDefault);
	lesion.setDefaultAttribute(lesionDefault);

	crown.setDefaultAttribute(prosthoDefault);
	bridge.setDefaultAttribute(prosthoDefault);
}
