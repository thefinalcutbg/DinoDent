#pragma once
#include <string>
#include <map>
#include <array>
#include <vector>
#include <set>
#include "ToothIndex.h"
#include "Dental.h"

typedef std::array<bool, Dental::SurfaceCount> SurfaceStatus;

class Tooth
{

	//teeth indexes are from 0 do 31
	const int m_index;
	std::array<bool, Dental::StatusCount> m_data { false };
	SurfaceStatus m_resto_surface = { false };
	SurfaceStatus m_caries_surface = { false };
	SurfaceStatus m_defect_surface = { false };
	SurfaceStatus m_non_caries{ false }; 

	std::map<int, std::string> m_lpkMap;

	//if the tooth is supernumeral, this is nullptr
	Tooth* m_supernumeral{ nullptr };
	//if the tooth is supernumeral it has parent 
	Tooth* m_parent{ nullptr };

	typedef std::array<std::vector<int>, Dental::StatusCount> IncompatibleCodes;

	static IncompatibleCodes incompatInit();
	static const IncompatibleCodes incompat_codes;
	static bool hasNoSurfacesSet(const decltype(m_resto_surface)& surface_arr);
	Dental::Surface getDefaultSurface() const;

	bool isHealthy() const;

	//HasSupernumeral constructor
	Tooth(Tooth* parent);

public:

	//holds the position if it is a Bridge or a splint
	Dental::BridgePos position{ Dental::Middle };
	//holds the mobility degree
	Dental::MobilityDegree m_degree{ Dental::I };

	Tooth(int arr_index);
	Tooth(const Tooth& other);
	Tooth& operator=(const Tooth& other);

	int index() const { return m_index; }
	ToothIndex toothIndex() const;
	Dental::Type type() const;
	Dental::Quadrant quadrant() const;
	bool isPontic() const;
	bool canHaveACrown() const;
	bool isWisdom() const;
	bool canHaveADenture() const;

	//code argument is either Status code or Surface - be careful!
	void setStatus(Dental::StatusType type, int code, bool present);

	//sets general status
	void setStatus(Dental::Status code, bool present = true);

	//use code Restoration or Code caries
	void setSurface(Dental::Status code, int surface, bool present = true);

	void setMobility(Dental::MobilityDegree degree, bool present);
	
	void clearStatuses();

	bool isSupernumeral() const { return m_supernumeral == nullptr; }

	//if the tooth is supernumeral it returns itself
	Tooth& getSupernumeral();
	const Tooth& getSupernumeral() const;

	bool operator[](Dental::Status s) const;

	bool hasStatus(int code) const;
	bool hasStatus(int code, int surface) const;
	
	bool hasSecondaryCaries(int surface) const;
	bool noData() const;

	const decltype(m_data)& getBoolStatus() const { return m_data; }
	const SurfaceStatus& getSurfaceBoolStatus(int code) const;

	std::array<bool, Dental::MobilityCount> getMobilityBoolStatus() const;

	//codes 0-5 for restoration surface; Others are same as the status codes.
	void setLPK(int code, const std::string& lpk);
	std::string getLPK(int code) const;

	void copyFromHIS(const Tooth& other);
	std::vector<std::string> getNhifStatus() const;
	std::vector<std::string> getHISStatus() const;
    std::string getPrintStatus() const;
    std::set<std::string> getDetailedPrintStatus() const;
    std::string getPrintLegend() const;
	std::string getToothInfo() const;
	std::string toothName() const;

	~Tooth();
};
