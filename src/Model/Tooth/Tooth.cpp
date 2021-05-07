#include "Tooth.h"



std::string Tooth::cariesDefault{"Caries media"};
std::string Tooth::pulpitisDefault{ "Pulpitis chronica ulcerosa" };
std::string Tooth::lesionDefault{"Periodontitis chronica granulomatosa"};
std::string Tooth::fractureDefault{ "Fractura coronae dentis" };
std::string Tooth::rootDefault{ "Radix dentis" };
std::string Tooth::obturationDefault{ "Фотополимер" };
std::string Tooth::prosthoDefault{ "Металокерамика" };

Tooth::Tooth() : 
	index{ -1 }
{
	for (int i = 0; i < 6; i++) {
		caries[i].setDefaultAttribute(cariesDefault);
		obturation[i].setDefaultAttribute(obturationDefault);
	}
	root.setDefaultAttribute(rootDefault);
	pulpitis.setDefaultAttribute(pulpitisDefault);
	fracture.setDefaultAttribute(fractureDefault);
	lesion.setDefaultAttribute(lesionDefault);

	crown.setDefaultAttribute(prosthoDefault);
	bridge.setDefaultAttribute(prosthoDefault);
	
	

}
