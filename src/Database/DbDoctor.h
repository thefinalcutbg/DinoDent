#pragma once

#include <unordered_map>
#include "Model/UserStructs.h"



namespace DbDoctor
{
	std::optional<Doctor>getDoctor(const std::string& lpk, const std::string& pass);
	std::optional<Doctor>getDoctor(const std::string& lpk);
	std::pair<std::string, std::string> getLpkAndPassAutoLogin();
	bool setAutoLogin(const std::string& lpk, bool remember);
	bool updateDoctor(const Doctor& doctor, std::string& currentLPK);
	bool insertDoctor(const Doctor& doctor);
	std::tuple<bool, int> getAdminAndSpecialty(const std::string& lpk, const std::string& rzi);
	std::unordered_map<std::string, std::string> getDoctorNames(); //returns LPK - name as key-value for all doctors in the db

}