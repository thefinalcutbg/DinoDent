#include "Tooth.h"



std::string Tooth::cariesDefault{"Caries media"};
std::string Tooth::pulpitisDefault{ "Pulpitis chronica ulcerosa" };
std::string Tooth::lesionDefault{"Periodontitis chronica granulomatosa"};
std::string Tooth::fractureDefault{ "Fractura coronae dentis" };
std::string Tooth::rootDefault{ "Radix dentis" };
std::string Tooth::obturationDefault{ u8"Фотополимер" };
std::string Tooth::prosthoDefault{ u8"Металокерамика" };

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



std::array<bool, statusCount> Tooth::getBoolArray() const
{
	return std::array<bool, statusCount>
	{
		temporary.exists(),
		obturation.exists(),
		caries.exists(),
		pulpitis.exists(),
		lesion.exists(),
		endo.exists(),
		post.exists(),
		root.exists(),
		fracture.exists(),
		extraction.exists(),
		periodontitis.exists(),
		mobility.exists() && mobility.degree == Degree::First,
		mobility.exists() && mobility.degree == Degree::Second,
		mobility.exists() && mobility.degree == Degree::Third,
		crown.exists(),
		bridge.exists(),
		implant.exists(),
		hyperdontic.exists()
	};
}




std::string Tooth::getSimpleStatus() const
{
	auto boolStatus = getBoolArray();

	std::array<std::string, statusCount> statusLegend
	{
		"", "O ", "C ", "P ", "G ", "", "", "R ", "F ", "E ",
		"Pa ", "I ", "II ", "III ", "K ", "X ", "I ", "Dsn "
	};

	if (boolStatus[StatusCode::Bridge]) //NZOK doesn't make a difference between a pontic and  a retainer :(
		boolStatus[StatusCode::Extraction] ?
			statusLegend[StatusCode::Extraction] = ""
			:
			statusLegend[StatusCode::Bridge] = "K ";
	

	std::string simpleStatus;

	for (int i = 0; i < statusCount; i++)
	{
		if (boolStatus[i])
			simpleStatus += statusLegend[i];
	}

	return simpleStatus;

}

