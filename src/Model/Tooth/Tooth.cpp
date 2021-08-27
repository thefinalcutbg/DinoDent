#include "Tooth.h"
#include <vector>
#include "ToothUtils.h"



Tooth::Tooth() : 
	index{ -1 }
{
	for (int i = 0; i < surfaceCount; i++)
	{
		caries[i].data.setDiagnosisList(DiagnosisContainer::caries());
	}

	pulpitis.data.setDiagnosisList(DiagnosisContainer::pulpitis());
	lesion.data.setDiagnosisList(DiagnosisContainer::lesion());
	fracture.data.setDiagnosisList(DiagnosisContainer::fracture());
	root.data.setDiagnosisList(DiagnosisContainer::root());
	

}

void Tooth::setIndex(int index)
{
	this->index = index;
	this->type = ToothUtils::getToothType(index);
}



std::array<bool, statusCount> Tooth::getBoolStatus() const
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
		hyperdontic.exists(),
		impacted.exists()
	};
}

std::string Tooth::getSimpleStatus() const
{
	auto boolStatus = getBoolStatus();

	std::array<std::string, statusCount> statusLegend
	{
		"", "O ", "C ", "P ", "G ", "", "", "R ", "F ", "E ",
		"Pa ", "I ", "II ", "III ", "K ", "X ", "I ", "Dsn "
	};

	if (boolStatus[StatusCode::Bridge]) //NZOK doesn't see a difference between a pontic and a retainer
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

//some free template functions to improve readability (hopefully)

template<class... Status> inline void set(bool state, Status&... parameters) { 
	(parameters.set(state), ...); 
}

template<typename T> void addSurface(SurfaceStatus<T>& status, int surface, Tooth& tooth){
	if (!status.exists()) set(false, tooth.extraction, tooth.root, tooth.implant);
	status.set(true, surface);
}

template<typename T> void removeSurface(SurfaceStatus<T>& status, int surface) { 
	status.set(false, surface); 
}

template<typename T> void removeAllSurfaces(SurfaceStatus<T>& status) { 
	status.reset(); 
}


void Tooth::addStatus(int statusCode)
{

	switch (statusCode)
	{
		case StatusCode::Temporary: 
			if (type == ToothType::Molar) break;
			set(true, temporary);  set(false, extraction, implant, post);  break;

		case StatusCode::Extraction:
			if (hyperdontic.exists() || impacted.exists()) { set(false, hyperdontic, impacted);  break; }
			if (temporary.exists()) { removeStatus(); temporary.set(false); break; }
			set(true, extraction); set(false, obturation, caries, implant, pulpitis, endo, fracture, root, lesion, periodontitis, crown, post, mobility); break;
   
		case StatusCode::Obturation: set(true, obturation); set(false, root, implant, extraction); break;
		case StatusCode::Caries: set(true, caries); set(false, root, implant, extraction); break;
		case StatusCode::Pulpitis: set(true, pulpitis); set(false, lesion, extraction, implant, endo, post); break;
		case StatusCode::EndoTreatment: set(true, endo); set(false, extraction, implant, pulpitis, impacted); break;
		case StatusCode::Post: set(true, post, endo); set(false, temporary, extraction, implant, pulpitis); break;
		case StatusCode::Root: set(true, root); set(false, caries, obturation, crown, extraction, implant); break;
		case StatusCode::Implant: set(true, implant); set(false, extraction, obturation, caries, hyperdontic, pulpitis, endo, fracture, root, post, mobility, impacted); break;
		case StatusCode::ApicalLesion: set(true, lesion); set(false, pulpitis, extraction); break;
		case StatusCode::Fracture: set(true, fracture); set(false, extraction, implant); break;
		case StatusCode::Periodontitis: set(true, periodontitis); set(false, extraction); break;
		case StatusCode::Mobility1: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::First; break;
		case StatusCode::Mobility2: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::Second; break;
		case StatusCode::Mobility3: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::Third; break;
		case StatusCode::Crown: set(true, crown); set(false, bridge, extraction, root, impacted); break;
		case StatusCode::Bridge: set(true, bridge); set(false, hyperdontic, crown); bridge.LPK.clear(); break;
		case StatusCode::Dsn: set(true, hyperdontic); set(false, extraction); break;
		case StatusCode::Impacted: if (!hyperdontic.exists()) set(false, extraction, implant, crown, post, endo, mobility);
						 impacted.set(true); break;

		default: break;
	};
	
}

void Tooth::removeStatus(int statusCode)
{
	switch (statusCode){
		case StatusCode::Temporary: temporary.set(false); break;
		case StatusCode::Obturation: obturation.set(false); break;
		case StatusCode::Caries: caries.set(false); break;
		case StatusCode::Pulpitis: pulpitis.set(false); break;
		case StatusCode::EndoTreatment:
		case StatusCode::Post: set(false, post, endo); break;
		case StatusCode::ApicalLesion: lesion.set(false); break;
		case StatusCode::Extraction: extraction.set(false); break;
		case StatusCode::Root: root.set(false); break;
		case StatusCode::Mobility1: 
		case StatusCode::Mobility2: 
		case StatusCode::Mobility3: mobility.set(false); break;
		case StatusCode::Implant: implant.set(false); break;
		case StatusCode::Fracture: fracture.set(false); break;
		case StatusCode::Crown: crown.set(false); break;
		case StatusCode::Bridge: bridge.set(false); break;
		case StatusCode::Periodontitis: periodontitis.set(false); break;
		case StatusCode::Dsn: hyperdontic.set(false); break;
		case StatusCode::Impacted: impacted.set(false); break;
		default: break;
	}
}

void Tooth::removeStatus() { 
	for(int i = 0; i < statusCount; i++) removeStatus(i); 
}

//public setters:

void Tooth::setStatus(StatusType type, int code, bool state)
{
	switch (type) {
		case StatusType::general: state ? addStatus(code) : removeStatus(code);  break;
		case StatusType::obturation: state ? addSurface(obturation, code, *this) : removeSurface(obturation, code); break;
		case StatusType::caries: state ? addSurface(caries, code, *this) : removeSurface(caries, code); break;
	}
}

void Tooth::setStatus(int code, bool state){
	setStatus(StatusType::general, code, state);
}

void Tooth::removeStatus(StatusType type)
{
	switch (type){
		case StatusType::general: removeStatus(); break;
		case StatusType::obturation: removeAllSurfaces(obturation); break;
		case StatusType::caries: removeAllSurfaces(caries); break;
	}
}




