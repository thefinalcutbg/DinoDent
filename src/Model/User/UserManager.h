#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "User.h"

struct Practice;
struct Doctor;
struct User;

namespace UserManager
{
	void initialize();
	
	const User& currentUser();
	const Practice& practice();
	Doctor& doctor();
	void setCurrentUser(const User& user);
	void setCurrentDoctor(const Doctor& doctor);
	void setPriceList(const std::vector<ProcedureTemplate>& priceList);
	void setCurrentPractice(const Practice& practice);
	void resetUser();
	const bool isCurrentUser(const std::string& LPK);
	std::string getDoctorName(const std::string& LPK);
	
};

