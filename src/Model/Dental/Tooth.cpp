#include "Tooth.h"
#include <vector>
#include "ToothUtils.h"
#include "View/Theme.h"


bool Tooth::isHealthyCheck()
{
	auto status = getBoolStatus();
	
	for (int i = 0; i < status.size(); i++) {

		if (i == StatusCode::Dsn ||
			i == StatusCode::Healthy ||
			i == StatusCode::Temporary
		) continue;

		if(status[i]) return false;
	}

	return true;

}

Tooth::Tooth(int index) : index(index), type(ToothUtils::getToothType(index)){}

Tooth::Tooth(const Tooth& other) : 
	index(other.index), 
	type(other.type),
	healthy(other.healthy),
	temporary(other.temporary),
	obturation(other.obturation),
	caries(other.caries),
	pulpitis(other.pulpitis),
	lesion(other.lesion),
	endo(other.endo),
	post(other.post),
	root(other.root),
	fracture(other.fracture),
	extraction(other.extraction),
	periodontitis(other.periodontitis),
	mobility(other.mobility),
	crown(other.crown),
	bridge(other.bridge),
	splint(other.splint),
	implant(other.implant),
	dsn(other.dsn),
	impacted(other.impacted),
	denture(other.denture),
	calculus(other.calculus)
{

}

Tooth& Tooth::operator=(const Tooth& other)
{
	healthy = other.healthy;
	temporary = other.temporary;
	obturation = other.obturation;
	caries = other.caries;
	pulpitis = other.pulpitis;
	lesion = other.lesion;
	endo = other.endo;
	post = other.post;
	root = other.root;
	fracture = other.fracture;
	extraction = other.extraction;
	periodontitis = other.periodontitis;
	mobility = other.mobility;
	crown = other.crown;
	bridge = other.bridge;
	splint = other.splint;
	implant = other.implant;
	dsn = other.dsn;
	impacted = other.impacted;
	denture = other.denture;
	calculus = other.calculus;

	return *this;
}

std::array<bool, statusCount> Tooth::getBoolStatus() const
{
	return std::array<bool, statusCount>
	{
		healthy.exists(),
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
		mobility.exists(),
		crown.exists(),
		bridge.exists(),
		splint.exists(),
		implant.exists(),
		dsn.exists(),
		impacted.exists(),
		denture.exists(),
		calculus.exists()
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
		"", "T", "O", "C", "P", "G", "", "", "R", "F", "E",
		"Pa", "I", "K", "K", "X", "Impl.", "Dsn", "", "X", ""
	};			  //     ^                        ^
				  //  bridge	                splint


	if (boolStatus[StatusCode::FiberSplint])
	{
		if (boolStatus[StatusCode::Extraction])
		{
			statusLegend[StatusCode::Extraction] = ""; //extraction won't be shown
		}
		else
		{
			boolStatus[StatusCode::Obturation] = true; //obturation WILL be shown
			statusLegend[StatusCode::FiberSplint] = "O"; //fibersplint WON'T be shown
		}
	}

	if (boolStatus[StatusCode::Mobility])
	{
		const char* mobility[3]{ "I", "II", "III" };
		statusLegend[StatusCode::Mobility] = mobility[static_cast<int>(this->mobility.degree)];
	}

	std::vector<std::string> simpleStatus;

	for (int i = 0; i < statusCount; i++)
	{
		if (boolStatus[i] && !statusLegend[i].empty())
			simpleStatus.push_back(statusLegend[i]);
	}

	return simpleStatus;
}

bool Tooth::noData() const
{
	for (auto status : getBoolStatus()) {
		if(status) return false;
	}

	return true;
}

//some free template functions to improve readability (hopefully)

template<class... Status> inline void set(bool state, Status&... parameters) { 
	(parameters.set(state), ...); 
}

template<typename T> void addSurface(SurfaceStatus<T>& status, int surface, Tooth& tooth){
	if (!status.exists()) set(false, tooth.healthy, tooth.extraction, tooth.root, tooth.implant, tooth.denture);
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
		case StatusCode::Healthy:
		{
			bool temp = temporary.exists();
			bool isDsn = dsn.exists();
			removeStatus();
			healthy.set(true);
			temporary.set(temp);
			dsn.set(isDsn);
			break;
		}
		case StatusCode::Temporary: 
			if (type == ToothType::Molar) break;
			set(true, temporary);  
			set(false, post); 
			healthy.set(isHealthyCheck());
			break;

		case StatusCode::Extraction:
//			if (temporary.exists()){ removeStatus(); temporary.set(false); break; }
			set(true, extraction);
			set(false, healthy, obturation, caries, implant, pulpitis, endo, fracture, root, lesion, periodontitis, crown, post, mobility, denture, calculus, impacted);
			break;

		case StatusCode::Obturation: 
			set(true, obturation); 
			set(false, healthy, root, implant, extraction, impacted, denture);
			break;

		case StatusCode::Caries: 
			set(true, caries); 
			set(false, healthy, root, implant, extraction, impacted, denture);
			break;

		case StatusCode::Pulpitis: 
			set(true, pulpitis); 
			set(false, healthy, lesion, extraction, implant, endo, post, impacted);
			if (denture && !root) denture.set(false);
			break;

		case StatusCode::EndoTreatment: 
			set(true, endo); 
			set(false, extraction, implant, pulpitis, impacted);
			if (denture && !root) denture.set(false); 
			break;

		case StatusCode::Post: 
			set(true, post, endo); 
			set(false, healthy, temporary, extraction, implant, pulpitis, impacted, denture);
			break;

		case StatusCode::Root: 
			set(true, root); 
			set(false, healthy, caries, obturation, crown, extraction, implant, calculus);
			break;

		case StatusCode::Implant: 
			set(true, implant); 
			set(false, temporary, healthy, extraction, obturation, caries, pulpitis, endo, fracture, root, post, mobility, splint, impacted, calculus);
			break;

		case StatusCode::ApicalLesion: 
			set(true, lesion); 
			set(false, healthy, pulpitis, extraction, impacted);
			if (denture && !root) denture.set(false);
			break;

		case StatusCode::Fracture: 
			set(true, fracture); 
			set(false, healthy, extraction, implant, impacted, crown);
			if (denture && !root) denture.set(false);
			break;

		case StatusCode::Periodontitis: 
			set(true, periodontitis); 
			set(false, healthy, extraction, impacted, denture);
			if (denture && !root) denture.set(false);  
			break;

		case StatusCode::Mobility: 
			set(true, mobility); 
			set(false, healthy, extraction, impacted);
			mobility.degree = Degree::First; break;
			if (denture && !root) denture.set(false);  
			break;

		case StatusCode::Crown: 
			set(true, crown); 
			set(false, healthy, bridge, extraction, root, splint, impacted, denture, fracture);
			break;

		case StatusCode::Bridge: 
			set(true, bridge); 
			set(false, healthy, crown, splint, denture);
			bridge.LPK.clear(); 
			break;

		case StatusCode::FiberSplint:
			set(true, splint); 
			set(false, healthy, crown, bridge, implant, denture);
			break;

		case StatusCode::Dsn: 
			if (noData()) { set(true, healthy); }
			set(true, dsn);
			if (dsn.toothNotNull() && dsn.tooth().noData()) dsn.tooth().healthy.set(true);
			break;

		case StatusCode::Impacted: 
			set(true, impacted); 
			set(false, healthy, obturation, caries, extraction, periodontitis, lesion, implant, crown, post, endo, mobility, fracture, calculus);
			break;

		case StatusCode::Denture: 
			set(true, denture); 
			set(false, healthy, obturation, caries, extraction, crown, bridge, splint, post, calculus);
			if (!root) {
				set(false, endo, lesion, pulpitis, periodontitis);
			}
			break;

		case StatusCode::Calculus:
			set(true, calculus);
			set(false, healthy, root, extraction, implant, impacted, denture);
			break;

		default: break;
	};
	
}

void Tooth::removeStatus(int statusCode)
{
	switch (statusCode){
		case StatusCode::Healthy: healthy.set(false); if (isHealthyCheck()) temporary.set(false); break;
		case StatusCode::Temporary: temporary.set(false); extraction.set(false); healthy.set(isHealthyCheck()); break;
		case StatusCode::Obturation: obturation.set(false); break;
		case StatusCode::Caries: caries.set(false); break;
		case StatusCode::Pulpitis: pulpitis.set(false); break;
		case StatusCode::EndoTreatment: set(false, post, endo); break;
		case StatusCode::Post: set(false, post); break;
		case StatusCode::ApicalLesion: lesion.set(false); break;
		case StatusCode::Extraction: extraction.set(false); break;
		case StatusCode::Root: root.set(false); if (denture) denture.set(false); break;
		case StatusCode::Mobility: mobility.set(false); break;
		case StatusCode::Implant: implant.set(false); break;
		case StatusCode::Fracture: fracture.set(false); break;
		case StatusCode::Crown: crown.set(false); break;
		case StatusCode::Bridge: bridge.set(false); break;
		case StatusCode::FiberSplint: splint.set(false); break;
		case StatusCode::Periodontitis: periodontitis.set(false); break;
		case StatusCode::Dsn: dsn.set(false); if (dsn.toothNotNull()) { dsn.tooth().removeStatus(); } break;
		case StatusCode::Impacted: impacted.set(false); if (denture || !root) denture.set(false); break;
		case StatusCode::Denture: denture.set(false); break;
		case StatusCode::Calculus: calculus.set(false); break;
		default: return;
	}

	if (statusCode != StatusCode::Healthy && isHealthyCheck()) healthy.set(true);
}

void Tooth::removeStatus() {

	for (int i = 0; i < statusCount; i++) {
		removeStatus(i);
	}

	healthy.set(false);
	
}

//public setters:

void Tooth::setStatus(StatusType type, int code, bool state)
{
	switch (type) {
		case StatusType::general: 
			state ? 
				addStatus(code) 
				: 
				removeStatus(code);  break;
		case StatusType::obturation: 
			state ? 
				addSurface(obturation, code, *this) 
				: 
				removeSurface(obturation, code); 

			break;
		case StatusType::caries: 
			state ? 
				addSurface(caries, code, *this) 
				: 
				removeSurface(caries, code); 
			break;
		case StatusType::mobility:
		{
			if (state) {
				addStatus(StatusCode::Mobility);
				mobility.degree = static_cast<Degree>(code);
			}
			else
			{
				removeStatus(StatusCode::Mobility);
			}
			
		}
		break;
	}

	if (code != StatusCode::Healthy && isHealthyCheck()) healthy.set(true);
}

void Tooth::setStatus(int code, bool state){
	setStatus(StatusType::general, code, state);
}

void Tooth::removeStatus(StatusType type)
{
	switch (type){
		case StatusType::general: removeStatus(); break;
		case StatusType::obturation: removeAllSurfaces(obturation);  break;
		case StatusType::caries: removeAllSurfaces(caries); break;
	}

	if (noData()) healthy.set(true);
}

std::string Tooth::toothName() const
{
	return ToothUtils::getName(index, temporary.exists());
}

bool Tooth::canHaveACrown() const
{
	return !extraction && !root && !impacted;
}


std::string Tooth::getToothInfo() const
{

	std::string result;

	if (dsn) result.append("<br><b>Свръхброен</b><br>");
	if (impacted) result.append("<br><b>Ретениран/в пробив</b><br>");
	

	if (endo) result.append("<br><b>Девитализиран</b><br>");

	if (obturation) {
		result.append("<br><b>Обтурация</b><br>");
		result.append(obturation.getInfo(index));
	}

	if (splint) {
		extraction ?
			result.append("<br><b>Възстановен с фибромост</b><br>")
			:
			result.append("<br><b>Шиниран с фибровлакно</b><br>");

		//result.append(splint.data.infoStr());
	}

	if (post) {

		//if (post.data.type.getIndex() == 0) {
			result.append("<br><b>Радикуларен щифт</b><br>");
		//}
		//else
		//{
		//	result.append("<br><b>" + post.data.type.getName() + "</b><br>");
		//}

	}
	

	if (caries) {
		result.append("<br><b><font color=\"red\">Кариес</font></b><br>");
		result.append(caries.getInfo(index));
	}

	if (crown) {
		result.append("<br><br><b>Корона</b><br>");
		//result.append(crown.data.infoStr());
	}

	if (bridge) {
		
		extraction || root || impacted ?
			result.append("<br><b>Мостово тяло</b><br>")
			:
			result.append("<br><b>Мостоносител</b><br>");

		//result.append(bridge.data.infoStr());
	}
	else if (extraction) {
		result.append("<br><b>Екстрахиран</b><br>");
	}

	if (implant) {
		result.append("<br><b>Имплант</b><br>");
	
	}

	if(pulpitis.exists()) result.append("<br>" + pulpitis.info());
	if (lesion.exists()) result.append("<br>" + lesion.info());
	if (fracture.exists()) result.append("<br>" + fracture.info());
	if (root.exists()) result.append("<br>" + root.info());
	//FIX PATHOLOGY!!!
	/*
	std::array<const Pathology*, 4> pato{
		&pulpitis, &lesion, &fracture, &root
	};

	for (auto p : pato) {
		if (!p->exists()) continue;

		result.append("<br>"+ p->info());
	}
	*/
	if (periodontitis) { result.append("<br><b><font color=\"red\">Пародонтит</font></b><br>");
}
	if (mobility) {

		result.append("<br><b><font color = \"red\">");

		switch (mobility.degree) {
			case Degree::First: result.append("Първа степен подвижност"); break;
			case Degree::Second: result.append("Втора степен подвижност"); break;
			case Degree::Third: result.append("Трета степен подвижност"); break;
		}

		result.append("</font></b><br>");
	}

	return result;


}




