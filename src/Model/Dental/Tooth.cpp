#include "Tooth.h"
#include <algorithm>
#include "ToothUtils.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace Dental;

Tooth::Tooth(int arr_index) : m_index(arr_index)
{
	if (arr_index < 0 || arr_index > 31) throw;

	m_supernumeral = new Tooth(this);
}

Tooth::Tooth(Tooth* parent) : m_index(parent->m_index), m_parent(parent)
{}

Tooth::Tooth(const Tooth& other) : m_index(other.m_index)
{
	m_data = other.m_data;
	m_caries_surface = other.m_caries_surface;
	m_resto_surface = other.m_resto_surface;
	m_defect_surface = other.m_defect_surface;
	m_non_caries = other.m_non_caries;
	m_degree = other.m_degree;
	position = other.position;
	m_lpkMap = other.m_lpkMap;
	m_parent = nullptr;

	if (other.m_supernumeral) {

		//recursive copy-constructor call for the supernumeral tooth
		m_supernumeral = new Tooth(*other.m_supernumeral);
		m_supernumeral->m_parent = this;
	}

}

Tooth& Tooth::operator=(const Tooth& other)
{
	if (this == &other) return *this;

	if (m_index != other.m_index) throw;

	m_data = other.m_data;
	m_caries_surface = other.m_caries_surface;
	m_resto_surface = other.m_resto_surface;
	m_defect_surface = other.m_defect_surface;
	m_non_caries = other.m_non_caries;
	m_degree = other.m_degree;
	position = other.position;
	m_lpkMap = other.m_lpkMap;

	//recursive assignment operator call
	if (other.m_supernumeral) {
		*m_supernumeral = *other.m_supernumeral;
	}

	return *this;
}

Tooth& Tooth::getSupernumeral()
{
	return m_supernumeral ? *m_supernumeral : *this;
}

const Tooth& Tooth::getSupernumeral() const
{
	return m_supernumeral ? *m_supernumeral : *this;;
}

void Tooth::copyFromHIS(const Tooth& other)
{
	m_data = other.m_data;
	m_caries_surface = other.m_caries_surface;
	m_resto_surface = other.m_resto_surface;
	m_defect_surface = other.m_defect_surface;
	m_non_caries = other.m_non_caries;
	m_degree = other.m_degree;

	if (other.m_lpkMap.size()) {
		m_lpkMap = other.m_lpkMap;
	}
}


bool Tooth::hasNoSurfacesSet(const decltype(m_resto_surface)& surface_arr)
{
	for (auto surface : surface_arr) {
		if (surface) return false;
	}

	return true;
}

Surface Tooth::getDefaultSurface() const
{
	return type() == Frontal ? Buccal : Occlusal;
}

bool Tooth::isHealthy() const
{
	for (int i = 2; i < m_data.size(); i++) {

		if (i == HasSupernumeral) continue;

		if (m_data[i]) return false;
	}

	return true;
}

Type Tooth::type() const
{
	return ToothUtils::getToothType(m_index);
}

Quadrant Tooth::quadrant() const
{
	return ToothUtils::getQuadrant(m_index);
}

ToothIndex Tooth::toothIndex() const
{
	return ToothIndex{
		.index = m_index,
		.temp = m_data[Temporary],
		.supernumeral = isSupernumeral()
	};
}


bool Tooth::hasStatus(int code) const
{
	if (code < 0 || code > m_data.size()) return false;

	return m_data[code];
}

bool Tooth::hasStatus(int code, int surface) const
{
	return getSurfaceBoolStatus(code)[surface];
}

bool Tooth::hasSecondaryCaries(int surface) const
{
	return hasStatus(Restoration, surface) && hasStatus(Caries, surface);
}

bool Tooth::isWisdom() const
{

	return
		m_index == 0 ||
		m_index == 15 ||
		m_index == 16 ||
		m_index == 31
		;
}

bool Tooth::canHaveADenture() const
{
	return
		( //is missing
			noData() ||
			m_data[Missing] ||
			m_data[Root] ||
			m_data[Implant] ||
			m_data[Impacted] ||
			m_data[Denture]
			) && // and it's not already restored by
		!m_data[Crown] &&
		!m_data[Bridge] &&
		!m_data[Splint]
		;

}


bool Tooth::noData() const
{
	for (auto s : m_data) {
		if (s) return false;
	}

	return true;
}

bool Tooth::isPontic() const
{
	return 	(m_data[Bridge] || m_data[Splint]) && (m_data[Missing] || m_data[Impacted]);
}

bool Tooth::canHaveACrown() const
{
	return  !m_data[Missing] &&
		!m_data[Root] &&
		!m_data[Impacted] &&
		!m_data[Denture]
		;
}

const SurfaceStatus& Tooth::getSurfaceBoolStatus(int code) const
{
	std::map<Status, const SurfaceStatus*> surfaceCodeMap = {
		{Restoration, &m_resto_surface},
		{Caries, &m_caries_surface},
		{NonCariesLesion, &m_non_caries},
		{DefectiveRestoration, &m_defect_surface}
	};

	static std::array<bool, 6> dummy = { false };

	if (!surfaceCodeMap.count(static_cast<Status>(code))) return dummy;

	return m_data[code] ? *surfaceCodeMap[static_cast<Status>(code)] : dummy;
}

std::array<bool, Dental::MobilityCount> Tooth::getMobilityBoolStatus() const
{
	std::array<bool, Dental::MobilityCount> result {0};

	if (m_data[Mobility]) result[m_degree] = true;

	return result;
}

void Tooth::setLPK(int code, const std::string& lpk)
{
	if (lpk.empty()) return;

	m_lpkMap[code] = lpk;
}

std::string Tooth::getLPK(int code) const
{
	if (m_lpkMap.count(code)) return m_lpkMap.at(code);
	return std::string();
}


Tooth::IncompatibleCodes Tooth::incompatInit()
{
	IncompatibleCodes result = {};

	//healthy has almost everything incompatible except temporary and supernumeral
	for (int i = Restoration; i < StatusCount; i++)
	{
		if (i == HasSupernumeral) continue;

		result[Healthy].push_back(static_cast<Status>(i));
	}

	result[Temporary] = { Post, Implant };
	result[Restoration] = { Healthy, Root, Implant, Missing, Impacted, Denture };
	result[DefectiveRestoration] = { Healthy, Root, Implant, Missing, Impacted, Denture };
	result[NonCariesLesion] = { Healthy, Root, Implant, Missing, Impacted, Denture };
	result[Caries] = { Healthy, Root, Implant, Missing, Impacted, Denture };
	result[NonCariesLesion] = { Healthy, Root, Implant, Missing, Impacted, Denture };
    result[Necrosis] = { Healthy, Pulpitis, Missing, Impacted, Implant, RootCanal, Post, Impacted, Denture };
    result[Resorption] = { Healthy, Implant, Missing, Denture };
    result[Pulpitis] = { Healthy, ApicalLesion, Necrosis, Missing, Impacted, RootCanal, Post, Impacted, Denture, Implant };
	result[ApicalLesion] = { Healthy, Pulpitis, Missing, Impacted, Implant, Denture };
    result[Missing] = { Healthy, Restoration, DefectiveRestoration, Caries, NonCariesLesion, Implant, Pulpitis, Necrosis, RootCanal, Resorption, Fracture, Root, ApicalLesion, Periodontitis, Crown, Post, Mobility, Denture, Calculus, Impacted };
	result[RootCanal] = { Healthy, Pulpitis, Necrosis, Missing, Impacted, Implant, Denture };
	result[Post] = { Healthy, Temporary, Missing, Implant, Pulpitis, Necrosis, Impacted, Denture };
	result[Root] = { Healthy, Caries, NonCariesLesion,  Restoration, DefectiveRestoration, Crown, Missing, Implant, Calculus };
    result[Implant] = { Healthy, ApicalLesion, Temporary, Missing, Restoration, DefectiveRestoration, Caries, NonCariesLesion,  Pulpitis, Necrosis, RootCanal, Resorption, Root, Post, Mobility, Impacted };
	result[Fracture] = { Healthy, Missing, Impacted };
	result[Periodontitis] = { Healthy, Missing, Impacted, Denture };
	result[Mobility] = { Healthy, Missing, Impacted, Denture };
	result[Crown] = { Healthy, Bridge, Missing, Root, Splint, Impacted, Denture };
	result[Bridge] = { Healthy, Crown, Splint, Denture };
	result[Splint] = { Healthy, Crown, Bridge, Denture };
	result[HasSupernumeral] = { };
	result[Impacted] = { Healthy, Restoration, DefectiveRestoration, Caries, NonCariesLesion, Missing, Periodontitis, ApicalLesion, Implant, Crown, Post, RootCanal, Mobility, Fracture, Calculus };
    result[Denture] = { Healthy, Restoration, DefectiveRestoration, Caries, NonCariesLesion, Missing, Crown, Bridge, Splint, Post, Calculus, ApicalLesion, RootCanal, Pulpitis, Necrosis, Resorption, Periodontitis, Mobility}; //if (!root)	set(false, endo, lesion, pulpitis, periodontitis);
	result[Calculus] = { Healthy, Root, Missing, Impacted, Denture };
		return result;

}

const Tooth::IncompatibleCodes Tooth::incompat_codes = incompatInit();

void Tooth::setStatus(Status code, bool present) {

	if (code == Temporary && type() == Molar) return;

	if (present == m_data[code]) return;

	//supernumeral cant have these statuses
	if (isSupernumeral() &&
		(
			code == Bridge ||
			code == Splint ||
			code == HasSupernumeral
			)
		)
	{
		return;
	}

	m_data[code] = present;

	//handling remove status
	if (!present) {

		if (code == RootCanal) { m_data[Post] = false; }

		if (code == HasSupernumeral && m_supernumeral) {
			m_supernumeral->clearStatuses();
		}

		if (code == Temporary) {

			bool bridge = m_data[Bridge];
			bool splint = m_data[Splint];
			
			for (auto& s : m_data) {
				s = false;
			}

			m_data[Bridge] = bridge;
			m_data[Splint] = splint;
		}

		m_data[Healthy] = isHealthy();

		if (m_parent) {
			m_parent->m_data[HasSupernumeral] = true;
		}

		return;
	}

	//handling add status

	//the edge case for placing Bridge over denture
	if (
		(code == Bridge || code == Splint) &&
		m_data[Denture] &&
		!m_data[Impacted] &&
		!m_data[Implant] &&
		!m_data[Root]
	) {
		m_data[Missing] = true;
	}

	for (auto c : incompat_codes[code]) {
		m_data[c] = false;
	}

	//notifying parent if supernumeral
	if (m_parent) {
		m_parent->setStatus(HasSupernumeral, true);
	}

	//some special cases:
	switch (code)
	{
	case Post:
        setStatus(RootCanal, true);
        break;
	case ApicalLesion:
		if (!m_data[RootCanal]) {
			setStatus(Necrosis, true);
		}
		break;
	case Restoration:
		if (hasNoSurfacesSet(m_resto_surface)) setSurface(Restoration, getDefaultSurface());
		break;

	case Caries:
		if (hasNoSurfacesSet(m_caries_surface)) setSurface(Caries, getDefaultSurface());
		break;

	case NonCariesLesion:
		if (hasNoSurfacesSet(m_non_caries)) setSurface(NonCariesLesion, Surface::Cervical);
		break;

	case DefectiveRestoration:
		if (hasNoSurfacesSet(m_defect_surface)) setSurface(DefectiveRestoration, getDefaultSurface());
		break;

	case Denture:
		if (!m_data[Root]) {
			m_data[RootCanal] = false;
			m_data[ApicalLesion] = false;
			m_data[Pulpitis] = false;
			m_data[Periodontitis] = false;
		}
		break;

	case HasSupernumeral:
		if (m_supernumeral->noData()) m_supernumeral->setStatus(Healthy, true);
		break;
	default:
		break;
	}

	m_data[Healthy] = isHealthy();

	if (m_parent) {
		m_parent->m_data[HasSupernumeral] = true;
	};


}

void Tooth::setSurface(Dental::Status code, int surface, bool present)
{
	std::map<Status, SurfaceStatus*> surfaceCodeMap = {
		{Restoration, &m_resto_surface},
		{Caries, &m_caries_surface},
		{NonCariesLesion, &m_non_caries},
		{DefectiveRestoration, &m_defect_surface}
	};

	static std::map<Status, std::vector<Status>> incompatSurfaceStatus = {
		{Restoration,			{DefectiveRestoration, NonCariesLesion}},
        {Caries,				{}},
        {DefectiveRestoration,	{Restoration, NonCariesLesion}},
        {NonCariesLesion,		{Restoration, DefectiveRestoration}}
	};

	if (!surfaceCodeMap.contains(code)) return;

	auto& arr = *surfaceCodeMap[code];

	if (present) {

		for (auto incompatCode : incompatSurfaceStatus[code]) {
			setSurface(incompatCode, surface, false);
		}

		//reseting the array
		if (!m_data[code]) {
			arr = { 0 };
		}

		arr[surface] = present;

		setStatus(code, true);

		return;
	}

	arr[surface] = present;

	for (auto s : arr) {
		if (s) return;
	}
	
	setStatus(code, false);

}

void Tooth::setMobility(MobilityDegree degree, bool present)
{
	setStatus(Mobility, present);

	if (present) {
		m_degree = degree;
		return;
	}

	m_degree = MobilityDegree::I;
}

void Tooth::setStatus(Dental::StatusType type, int code, bool present)
{
	switch (type)
	{
	case StatusType::General:
		setStatus((Status)code, present); break;
	case StatusType::Restoration:
		setSurface(Restoration, (Surface)code, present); break;
	case StatusType::Caries:
		setSurface(Caries, (Surface)code, present); break;
	case StatusType::DefectiveRestoration:
		setSurface(DefectiveRestoration, (Surface)code, present); break;
	case StatusType::NonCariesLesion:
		setSurface(NonCariesLesion, (Surface)code, present); break;
	case StatusType::Mobility:
		setMobility((MobilityDegree)code, present); break;
	}
}

void Tooth::clearStatuses()
{
	m_data = { 0 };

	if (m_parent) {
		m_parent->m_data[HasSupernumeral] = false;
	}
	else {
		m_supernumeral->m_data = { 0 };
	}

}

bool Tooth::operator[](Dental::Status s) const
{
	if (s < 0 || s > StatusCount) return false;

	return m_data[s];
}


std::string Tooth::toothName() const
{
	return ToothUtils::getName(m_index, m_data[Temporary]);
}

std::vector<std::string> Tooth::getNhifStatus() const
{
	std::array<std::string, StatusCount> statusLegend = { "" };
	static constexpr std::array<const char*, 3> mobilityNum{ "I", "II", "III" };

	statusLegend[Temporary] = "T";
	statusLegend[Restoration] = "O";
	statusLegend[Caries] = "C";
	statusLegend[Pulpitis] = "P";
	statusLegend[DefectiveRestoration] = "O";
	statusLegend[Necrosis] = "P";
	statusLegend[ApicalLesion] = "G";
	statusLegend[Root] = "R";
	statusLegend[Missing] = m_data[Splint] || m_data[Bridge] ? "" : "E";
	statusLegend[Crown] = "K";
	statusLegend[Bridge] = isPontic() ? "X" : "K";
	statusLegend[Denture] = "X";
	statusLegend[Periodontitis] = "Pa";
	statusLegend[Mobility] = m_data[Mobility] ? statusLegend[Mobility] = mobilityNum[static_cast<int>(m_degree)] : "";
	statusLegend[Fracture] = "F";
	statusLegend[Implant] = "Impl.";
	statusLegend[HasSupernumeral] = isSupernumeral() ? "Dsn" : "";
	statusLegend[Splint] = isPontic() ? "X" : "O";

	std::vector<std::string> simpleStatus;

	for (int i = 0; i < StatusCount; i++)
	{

		if (i == HasSupernumeral && isSupernumeral()) {
			simpleStatus.push_back("Dsn");
			continue;
		}

		if (m_data[i] && !statusLegend[i].empty())
			simpleStatus.push_back(statusLegend[i]);
	}

	return simpleStatus;
}

std::vector<std::string> Tooth::getHISStatus() const
{
	std::vector<std::string> statuses{};

	static std::array<std::string, 6> surfNum{ "o", "m", "d", "b", "l", "c" };
	static constexpr std::array<const char*, 3> mobilityNum{ "M1", "M2", "M3" };
	std::array<std::string, StatusCount> status;

	status[Healthy] = "H";
	status[Temporary] = "";
	status[Restoration] = "O";
	status[Necrosis] = "N";
	status[Resorption] = "Res";
	status[DefectiveRestoration] = "DR";
	status[NonCariesLesion] = "NC";
	status[Caries] = "C";
	status[Pulpitis] = "P";
	status[ApicalLesion] = "G";
	status[Root] = "R";
	status[Missing] = "E";
	status[Crown] = "K";
	status[Bridge] = isPontic() ? "B" : "Kb";
	status[Denture] = "X";
	status[Periodontitis] = "Pa";
	status[Mobility] = "";
	if (m_data[Mobility]) {
		status[Mobility] = mobilityNum[static_cast<int>(m_degree)];
	}
	status[Fracture] = "F";
	status[Implant] = "I";
	status[HasSupernumeral] = "D";
	status[Impacted] = "Re";
	status[RootCanal] = "Rc";
	status[Post] = "Rp";
	status[Splint] = "S";
	status[Calculus] = "T";

	auto boolStatus = getBoolStatus();

	//HIS requires supernumeral to be property of the supernumeral tooth
	boolStatus[HasSupernumeral] = isSupernumeral();

	//adding surfaces
	for (int i = Restoration; i <= NonCariesLesion; i++) {
		for (int y = 0; y < SurfaceCount; y++) {
			if (hasStatus(i, y)) statuses.push_back(status[i] + surfNum[y]);
		}
	}

	for (int i = 0; i < StatusCount; i++) {

		//skipping since already added
		if (i >= Temporary && i <= NonCariesLesion) {
			continue;
		}

		if (i == HasSupernumeral && isSupernumeral()) {
			statuses.push_back(status[HasSupernumeral]);
			continue;
		}

		if (boolStatus[i] && status[i].size()) {
			statuses.push_back(status[i]);
		}
	}

	return statuses;
}

std::string Tooth::getPrintStatus() const
{
	auto vec = getNhifStatus();

	std::string result;
	result.reserve(vec.size() * 3);

	for (auto& s : vec)
	{
		if (s == "T") continue;

		result += s;
		result += " ";
	}

	if (!result.empty()) {

		result.pop_back(); //removing the last interval
	}

	return result;

}

std::set<std::string>  Tooth::getDetailedPrintStatus() const
{
    auto vec = getHISStatus();

    static const std::unordered_map<std::string, std::string> simplificationMap = [] {
        std::unordered_map<std::string, std::string> m;
        static const std::array<const char*, 6> surf{ "o", "m", "d", "b", "l", "c" };

        for (auto s : surf) {
            m.emplace(std::string("C")  + s, "C");
            m.emplace(std::string("O")  + s, "O");
            m.emplace(std::string("DR") + s, "DR");
            m.emplace(std::string("NC") + s, "NC");
        }
        return m;
    }();

    std::set<std::string> result;

    for (const auto& status : vec) {

        if(simplificationMap.contains(status)){
            result.insert(simplificationMap.at(status));
        } else {
            result.insert(status);
        }
    }

    return result;
}

std::string Tooth::getToothInfo() const
{
	auto status = getBoolStatus();

	std::string result;

	result.reserve(200);

	for (int i = 0; i < status.size(); i++)
	{
		if (status[i] && i != Mobility) {

			result.append("<br><b>");
			result.append(statusNames[i]);
			result.append("</b>");
		}

		if (i == Restoration ||
			i == Caries ||
			i == DefectiveRestoration ||
			i == NonCariesLesion
			) {
		
			if (!hasStatus(i)) continue;

			auto& surfaces = getSurfaceBoolStatus(i);

			for (int y = 0; y < SurfaceCount; y++)
			{
				if (!surfaces[y]) continue;
				result.append("<br>");
				result.append(surfaceNames[y]);
			}
		}

		if (i == Mobility && (*this)[Mobility]) {

			result.append("<br><b>");
			result.append(mobilityNames[m_degree]);
			result.append("</b>");

		}

	}

	auto numenclature = getHISStatus();

	if (!numenclature.size()) {
		result += "<br><b>Статус:</b> Неизвестен";
		return result;
	}

	result.append("<br><br><b>Кодове: </b>");

	for (auto& num : numenclature)
	{
		result.append(num);
		result.append(", ");
	}

	result.pop_back();
	result.pop_back();

	return result;
}

Tooth::~Tooth()
{
	if (m_supernumeral) {
		delete m_supernumeral;
	}
}
