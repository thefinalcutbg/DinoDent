﻿#include "Tooth.h"
#include <vector>
#include "ToothUtils.h"



Tooth::Tooth() : 
	index{ -1 }
{
	for (int i = 0; i < surfaceCount; i++)
	{
		caries[i].data.setDiagnosisList(diagnosis(DiagnosisContainer::DiagnosisType::Caries));
	}

	pulpitis.data.setDiagnosisList(diagnosis(DiagnosisContainer::DiagnosisType::Pulpitis));
	lesion.data.setDiagnosisList(diagnosis(DiagnosisContainer::DiagnosisType::Lesion));
	fracture.data.setDiagnosisList(diagnosis(DiagnosisContainer::DiagnosisType::Fracture));
	root.data.setDiagnosisList(diagnosis(DiagnosisContainer::DiagnosisType::Root));

}

void Tooth::setIndex(int index)
{
	this->type = ToothUtils::getToothType(index);
	this->index = index;

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
		splint.exists(),
		implant.exists(),
		hyperdontic.exists(),
		impacted.exists()
	};
}

std::string Tooth::getStringStatus() const
{
	auto vec = getSimpleStatuses();

	std::string result;
	result.reserve(vec.size() * 3);

	for (auto s : vec)
	{
		if (s == "T") continue;

		result += s;
		result += " ";
	}

	if (!result.empty()){

		result.pop_back(); //removing the last interval
	}

	return result;

}



std::vector<std::string> Tooth::getSimpleStatuses() const
{
	auto boolStatus = getBoolStatus();

	std::array<std::string, statusCount> statusLegend //each letter corresponds to bool status
	{
		"T", "O", "C", "P", "G", "", "", "R", "F", "E",
		"Pa", "I", "II", "III", "K", "X", "X", "Impl.", "Dsn", ""
	};							//     ^     ^
								//  bridge	splint (both defaulted as artificial tooth)

	if (boolStatus[StatusCode::Bridge])
	{
		boolStatus[StatusCode::Extraction] ?
			statusLegend[StatusCode::Extraction] = "" //extraction won't be shown
			:
			statusLegend[StatusCode::Bridge] = "K"; //bridge will be shown as K

	}

	if (boolStatus[StatusCode::FiberSplint])
	{
		if (boolStatus[StatusCode::Extraction])
		{
			statusLegend[StatusCode::Extraction] = ""; //extraction won't be shown
		}
		else
		{
			boolStatus[StatusCode::Obturation] = true; //obturation WILL be shown
			statusLegend[StatusCode::FiberSplint] = ""; //fibersplint WON'T be shown
		}
	}

	std::vector<std::string> simpleStatus;

	for (int i = 0; i < statusCount; i++)
	{
		if (boolStatus[i] && !statusLegend[i].empty())
			simpleStatus.push_back(statusLegend[i]);
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
		case StatusCode::Implant: set(true, implant); set(false, temporary, extraction, obturation, caries, hyperdontic, pulpitis, endo, fracture, root, post, mobility, splint, impacted); break;
		case StatusCode::ApicalLesion: set(true, lesion); set(false, pulpitis, extraction); break;
		case StatusCode::Fracture: set(true, fracture); set(false, extraction, implant, impacted); break;
		case StatusCode::Periodontitis: set(true, periodontitis); set(false, extraction); break;
		case StatusCode::Mobility1: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::First; break;
		case StatusCode::Mobility2: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::Second; break;
		case StatusCode::Mobility3: set(true, mobility); set(false, extraction, impacted); mobility.degree = Degree::Third; break;
		case StatusCode::Crown: set(true, crown); set(false, bridge, extraction, root, splint); break;
		case StatusCode::Bridge: set(true, bridge); set(false, hyperdontic, crown, splint); bridge.LPK.clear(); break;
		case StatusCode::FiberSplint: set(true, splint); set(false, crown, bridge, implant); break;
		case StatusCode::Dsn: set(true, hyperdontic); set(false, extraction); break;
		case StatusCode::Impacted: if (!hyperdontic.exists()) set(false, extraction, implant, crown, post, endo, mobility, fracture);
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
		case StatusCode::EndoTreatment: set(false, post, endo); break;
		case StatusCode::Post: set(false, post); break;
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
		case StatusCode::FiberSplint: splint.set(false); break;
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

std::string Tooth::toothName() const
{
	return ToothUtils::getName(index, temporary.exists());
}

std::string Tooth::getToothInfo()
{

	std::string result;

	if (hyperdontic) result.append(u8"<br><b>Свръхброен</b><br>");
	if (impacted) result.append(u8"<br><b>Ретениран/в пробив</b><br>");
	

	if (endo) result.append(u8"<br><b>Девитализиран</b><br>");

	if (obturation) {
		result.append(u8"<br><b>Обтурация</b><br>");
		result.append(obturation.getInfo(index));
	}

	if (splint) {
		extraction ?
			result.append(u8"<br><b>Възстановен с фибромост</b><br>")
			:
			result.append(u8"<br><b>Шиниран с фибровлакно</b><br>");

		result.append(splint.data.infoStr());
	}

	if (post) result.append(u8"<br><b>Радикуларен щифт</b><br>");
	

	if (caries) {
		result.append(u8"<br><b><font color=\"red\">Кариес</font></b><br>");
		result.append(caries.getInfo(index));
	}

	if (crown) {
		result.append(u8"<br><br><b>Корона</b><br>");
		result.append(crown.data.infoStr());
	}

	if (bridge) {
		
		extraction ?
			result.append(u8"<br><b>Мостово тяло</b><br>")
			:
			result.append(u8"<br><b>Мостоносител</b><br>");

		result.append(bridge.data.infoStr());
	}
	else if (extraction) {
		result.append(u8"<br><b>Екстрахиран</b><br>");
	}

	if (implant) {
		result.append(u8"<br><b>Имплант</b><br>");
		result.append(implant.data.infoStr());
	}

	std::array<Pathology*, 4> pato{
		&pulpitis, &lesion, &fracture, &root
	};

	for (auto p : pato) {
		if (!p->exists()) continue;

		result.append("<br><b><font color=\"red\">" + p->info() + "</font></b><br>"
					+u8" (диагностициран на " + p->data.date_diagnosed.toBgStandard(true) + ")<br>");
	}

	if (periodontitis) { result.append(u8"<br><b><font color=\"red\">Пародонтит</font></b><br>");
}
	if (mobility) {

		result.append("<br><b><font color = \"red\">");

		switch (mobility.degree) {
		case Degree::First: result.append(u8"Първа степен подвижност"); break;
		case Degree::Second: result.append(u8"Втора степен подвижност"); break;
		case Degree::Third: result.append(u8"Трета степен подвижност"); break;
		}

		result.append("</font></b><br>");
	}

	return result;


}



