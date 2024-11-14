#pragma once

#include <unordered_map>
#include <set>
#include "Model/UserStructs.h"



namespace DbDoctor
{
	struct UserCredentials {
		std::string lpk;
		std::string pass;
		std::string name;
	};

	std::optional<Doctor>getDoctor(const std::string& lpk, const std::string& pass);
	std::optional<Doctor>getDoctor(const std::string& lpk);
	std::string getDoctorAutoLoginLPK();
	bool setAutoLogin(const std::string& lpk, bool remember);
	bool updateDoctor(const Doctor& doctor, const std::string& currentLPK);
	bool insertDoctor(const Doctor& doctor);
	bool suchDoctorExists(const std::string& LPK);
	std::tuple<bool, int> getAdminAndSpecialty(const std::string& lpk, const std::string& rzi);
	std::unordered_map<std::string, std::string> getDoctorNames(); //returns LPK - name as key-value for all doctors in the db
	bool updateFavouriteProcedures(const std::vector<std::string>& procedureCodes, const std::string& doctorLPK);
	std::set<std::string> getFavouriteProcedures(const std::string& doctorLPK);
	std::vector<UserCredentials> getDoctorList();
	bool isIncognito(const std::string& lpk);
	bool setIncognito(bool incognito, const std::string& lpk);
	bool setCalendarRefreshToken(const std::string& token, const std::string& lpk);
	std::string calendarRefreshToken(const std::string& lpk);
	bool setCurrentCalendarIdx(int idx, const std::string& lpk);
	int currentCalendarIdx(const std::string& lpk);
}