#pragma once

#include "AbstractORM.h"
#include <vector>
#include "Model/User/User.h"
#include <optional>
#include <unordered_map>
#include "Model/Procedure/ProcedureTemplate.h"

class DbLogin : public AbstractORM
{
public:
	std::vector<PracticePair> getPracticeList(const std::string& doctorLPK);
	Practice getPractice(const std::string rziCode);
	void updatePractice(const Practice& practice, const std::string& currentRZI);
	void insertPractice(const Practice& practice);
	std::optional<Doctor>getDoctor(const std::string& lpk, const std::string& pass);
	std::optional<Doctor>getDoctor(const std::string& lpk);
	std::unordered_map<std::string, std::string> getDoctorNames(); //returns LPK - name as key-value for all doctors in the db
	std::vector<PracticeDoctor> getDoctors(const std::string& practiceRZI);
	void setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& practiceRZI);
	void updateDoctor(const Doctor& doctor, std::string& currentLPK);
	void updatePriceList(const std::vector<ProcedureTemplate>& priceList, const std::string& rziCode);
	void insertDoctor(const Doctor& doctor);
	bool getAdminPremission(const std::string& lpk, const std::string& rzi);
	bool practiceExists(const std::string& rzi);
	bool noPractices();
};

