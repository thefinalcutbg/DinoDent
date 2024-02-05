#pragma once
#include <string>
#include <map>
#include <array>
#include <vector>

#include "ToothIndex.h"
#include "Dental.h"


class Tooth
{

private:

	//teeth indexes are from 0 do 31
	const int m_index;

	std::array<bool, Dental::StatusCount> m_data { false };

	std::array<bool, Dental::SurfaceCount> m_resto_surface = { false };
	std::array<bool, Dental::SurfaceCount> m_caries_surface = { false };



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

	//holds the position if it is a bridge or a splint
	Dental::BridgePos position{ Dental::Middle };
	//holds the mobility degree
	Dental::MobilityDegree m_degree{ Dental::I };

	Tooth(int arr_index);
	Tooth(const Tooth& other);
	Tooth& operator=(const Tooth& other);

	int index() const { return m_index; }

	//code is either Status code or Surface - be careful!
	void setStatus(Dental::StatusType type, int code, bool present);
	void setStatus(Dental::Status code, bool present = true);
	void setSurface(Dental::Status code, int surface, bool present = true);
	void setMobility(Dental::MobilityDegree degree, bool present = true);
	void clearStatuses();
	bool isSupernumeral() const { return m_supernumeral == nullptr; }

	Tooth& getSupernumeral();

	const Tooth& getSupernumeral() const;
	Dental::Type type() const;
	Dental::Quadrant quadrant() const;
	ToothIndex toothIndex() const;

	bool operator[](Dental::Status s) const;

	bool hasStatus(int code) const;
	bool hasRestoration(int surface) const;
	bool hasCaries(int surface) const;
	bool hasMobility(Dental::MobilityDegree degree) const;
	bool hasSecondaryCaries(int surface) const;
	bool isWisdom() const;
	bool noData() const;
	bool isPontic() const;
	bool canHaveACrown() const;

	const decltype(m_data)& getBoolStatus() const { return m_data; }
	std::array<bool, Dental::SurfaceCount> getRestorationBoolStatus() const;
	std::array<bool, Dental::SurfaceCount> getCariesBoolStatus() const;
	std::array<bool, Dental::MobilityCount> getMobilityBoolStatus() const;

	void setLPK(int code, const std::string& lpk);
	std::string getLPK(int code) const;

	std::string toothName() const;

	void copyFromHIS(const Tooth& other);
	std::vector<std::string> getNhifStatus() const;
	std::vector<std::string> getHISStatus() const;
	std::string getPrintStatus() const;
	std::string getToothInfo() const;

	~Tooth();
};