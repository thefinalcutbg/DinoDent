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

std::string Tooth::getPrintStatus() const
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
	};			  //     ^                         ^
				  //  bridge	                 splint


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

	if (boolStatus[StatusCode::Bridge])
	{
		if(boolStatus[StatusCode::Extraction] || boolStatus[StatusCode::Impacted])
		{
			statusLegend[StatusCode::Extraction] = "";
			statusLegend[StatusCode::Bridge] = "X";
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

std::vector<std::string> Tooth::getHISStatus() const
{
	std::vector<std::string> statuses;

	static constexpr std::array<const char*, surfaceCount> cariesNum{ "Co", "Cm", "Cd", "Cb", "Cl" , "Cc" };
	static constexpr std::array<const char*, surfaceCount> obturationNum{ "Oo", "Om", "Od", "Ob", "Ol", "Oc" };
	static constexpr std::array<const char*, mobilityCount> mobilityNum{ "M1", "M2", "M3" };
	std::array<const char*, statusCount> status;

	status[StatusCode::Healthy] = "H";
	status[StatusCode::Temporary] = "";
	status[StatusCode::Obturation] = "";
	status[StatusCode::Caries] = "";
	status[StatusCode::Pulpitis] = "P";
	status[StatusCode::ApicalLesion] = "G";
	status[StatusCode::Root] = "R";
	status[StatusCode::Extraction] = "E";
	status[StatusCode::Crown] = "K";
	status[StatusCode::Bridge] = isPontic() ? "B" : "Kb";
	status[StatusCode::Denture] = "X";
	status[StatusCode::Periodontitis] = "Pa";
	status[StatusCode::Mobility] = "";
	if (mobility) {
		status[StatusCode::Mobility] = mobilityNum[static_cast<int>(mobility.degree)];
	}
	status[StatusCode::Fracture] = "F";
	status[StatusCode::Implant] = "I";
	status[StatusCode::Dsn] = "D";
	status[StatusCode::Impacted] = "Re";
	status[StatusCode::EndoTreatment] = "Rc";
	status[StatusCode::Post] = "Rp";
	status[StatusCode::FiberSplint] = "S";
	status[StatusCode::Calculus] = "T";

	auto boolStatus = getBoolStatus();

	for (int i = 0; i < surfaceCount; i++) {

		if (caries.exists(i)) statuses.push_back(cariesNum[i]);
		if (obturation.exists(i)) statuses.push_back(obturationNum[i]);

	}

	for (int i = 0; i < statusCount; i++) {
		if (i == StatusCode::Caries || i == StatusCode::Obturation || i == StatusCode::Temporary) continue;
		if (boolStatus[i]) statuses.push_back(status[i]);
	}

	return statuses;
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
			healthy.set(true);
			set(false, obturation, caries, pulpitis, lesion, endo, post, root, fracture, extraction, periodontitis, mobility, crown, bridge, splint, implant, impacted, denture, calculus);
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
			set(false, extraction, implant, pulpitis, impacted, healthy);
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
			set(false, temporary, healthy, extraction, obturation, caries, pulpitis, endo, fracture, root, post, mobility, impacted, calculus);
			break;

		case StatusCode::ApicalLesion: 
			set(true, lesion); 
			set(false, healthy, pulpitis, extraction, impacted);
			if (denture && !root) denture.set(false);
			break;

		case StatusCode::Fracture: 
			set(true, fracture); 
			set(false, healthy, extraction, implant, impacted);
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
			set(false, healthy, bridge, extraction, root, splint, impacted, denture);
			break;

		case StatusCode::Bridge: 
			set(true, bridge); 
			set(false, healthy, crown, splint, denture);
			bridge.LPK.clear(); 
			break;

		case StatusCode::FiberSplint:
			set(true, splint); 
			set(false, healthy, crown, bridge, denture);
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

	if (isHealthyCheck()) healthy.set(true);

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

bool Tooth::isPontic() const
{
	if (!bridge) return false;

	return extraction || impacted;
}

bool Tooth::canHaveACrown() const
{
	return !extraction && !root && !impacted && !denture;
}

std::string Tooth::getToothInfo() const
{
	auto status = getBoolStatus();

	std::string result;

	result.reserve(200);

	for (int i = 0; i < status.size(); i++)
	{
		if (status[i] && i != StatusCode::Mobility) {

			result.append("<br><b>");
			result.append(statusNames[i]);
			result.append("</b>");
		}

		if (i == StatusCode::Obturation && obturation) {

			result.append(":");

			auto surfaces = obturation.getBoolStatus();

			for (int y = 0; y < surfaceCount; y++)
			{
				if (!surfaces[y]) continue;
				result.append("<br>");
				result.append(surfaceNames[y]);
			}
		}

		if (i == StatusCode::Caries && caries) {

			result.append(":");

			auto surfaces = caries.getBoolStatus();

			for (int y = 0; y < surfaceCount; y++)
			{
				if (!surfaces[y]) continue;
				result.append("<br>");
				result.append(surfaceNames[y]);
				
			}
		}

		if (i == StatusCode::Mobility && mobility) {

			result.append("<br><b>");
			result.append(mobilityNames[static_cast<int>(mobility.degree)]);
			result.append("</b>");

		}
		
	}

	auto numenclature = getHISStatus();

	if (!numenclature.size()) {
		result += "<br><b>Статус:</b> Неизвестен";
		return result;
	}

	result.append("<br><br><b>Кодове: </b>");

	for (auto& num : getHISStatus())
	{
		result.append(num);
		result.append(", ");
	}

	result.pop_back();
	result.pop_back();

	return result;

}




