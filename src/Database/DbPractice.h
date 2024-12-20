#pragma once
#include <vector>
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

	//void updatePriceList(const std::vector<ProcedureCode>& priceList, const std::string& rhiCode);

	bool practiceExists(const std::string& rzi);
	bool noPractices();
}
