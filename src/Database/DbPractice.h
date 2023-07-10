#pragma once
#include <vector>
#include "Model/UserStructs.h"


namespace DbPractice
{
	Practice getPractice(const std::string rziCode);
	void updatePractice(const Practice& practice, const std::string& currentRZI);
	void updatePracticeSettings(const Settings& s, const std::string& rzi);
	void insertPractice(const Practice& practice);
	bool deletePractice(const std::string& rziCode);
	std::vector<PracticePair> getPracticeList(const std::string& doctorLPK = {});
	void setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& practiceRZI);
	std::vector<PracticeDoctor> getDoctors(const std::string& practiceRZI);

	//void updatePriceList(const std::vector<ProcedureCode>& priceList, const std::string& rziCode);

	bool practiceExists(const std::string& rzi);
	bool noPractices();
}
