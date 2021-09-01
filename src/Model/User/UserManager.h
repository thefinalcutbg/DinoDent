#pragma once

#include <vector>
#include <optional>
#include "User.h"

class UserManager
{
	std::vector<User> m_users;

	static UserManager m_instance;
	static User m_currentUser;

	UserManager();

public:
	static inline UserManager& instance() { return m_instance; }
	static inline const User& currentUser() { return m_currentUser; }
	const std::optional<User> getUser(const std::string& LPK) const;
	const bool isCurrentUser(const std::string& LPK);
	std::string getUserTitle(const std::string& LPK) const;
	
};

