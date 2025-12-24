#pragma once
#include <vector>
#include <set>
#include "Model/UserStructs.h"

namespace DbPractice
{
	Practice getPractice(const std::string rhiCode);
	bool updatePractice(const Practice& practice, const std::string& currentRhiCode);
	bool updatePracticeSettings(const Settings& s, const std::string& rhiCode);
	bool insertPractice(const Practice& practice);
	bool deletePractice(const std::string& rziCode);
	std::vector<PracticePair> getPracticeList(const std::string& doctorLPK = {});
	bool setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& rhiCode);
	std::vector<PracticeDoctor> getDoctors(const std::string& rhiCode);
    std::set<int> getUnfavEkatte(const std::string& rziCode);
    void setUnfavEkatte(const std::set<int>& unfavEkatte, const std::string& rziCode);

    std::unordered_map<std::string, std::pair<double, double>> getCodeValues(const std::string& rziCode);
    std::pair<double, double> getCodeValue(const std::string& code, const std::string& rziCode);
    bool setCodeValues(const std::string& code, const std::pair<double, double>& priceRange, const std::string& rziCode);

	bool practiceExists(const std::string& rzi);
	bool noPractices();
}
