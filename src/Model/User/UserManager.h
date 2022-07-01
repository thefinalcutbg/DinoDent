#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "User.h"


class UserManager
{
	static inline std::unordered_map<std::string, std::string> m_names;

	static UserManager m_instance;
	static User m_currentUser;

	UserManager();

public:
	static void initialize();
	static inline UserManager& instance() { return m_instance; }
	static inline const User& currentUser() { return m_currentUser; }
	static inline const Practice& practice() { return m_currentUser.practice; }
	static inline const Doctor& doctor() { return m_currentUser.doctor; }
	static inline void setCurrentUser(const User& user) { m_currentUser = user; }
	static inline void setCurrentDoctor(const Doctor& doctor) { m_currentUser.doctor = doctor; }
	static inline void setPriceList(const std::vector<ProcedureTemplate>& priceList) { m_currentUser.practice.priceList = priceList; }
	static inline void setCurrentPractice(const Practice& practice) { m_currentUser.practice = practice; };
	static inline void resetUser() { m_currentUser = User{}; }
	const bool isCurrentUser(const std::string& LPK);
	static std::string getDoctorName(const std::string& LPK);
	
};

