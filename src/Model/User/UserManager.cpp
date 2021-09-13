#include "UserManager.h"

UserManager UserManager::m_instance;
User UserManager::m_currentUser;

void UserManager::initialize()
{
    m_currentUser = User{ u8"Христо", u8"Константинов", "220008771", 64, u8"София, общ. Столична, обл. София-град" };
    m_instance.m_users.push_back(m_currentUser);
}

UserManager::UserManager()
{

}

const std::optional<User> UserManager::getUser(const std::string& LPK) const
{
    std::optional<User> user;

    for (auto& u : m_users)
    {
        if (u.LPK == LPK)
            user = u;
    }

    return user;

}

const bool UserManager::isCurrentUser(const std::string& LPK)
{
    return LPK == m_currentUser.LPK;
}

std::string UserManager::getUserTitle(const std::string& LPK) const
{
    for (auto& u : m_users)
    {
        if (u.LPK != LPK) continue;

        std::string name = u8"д-р ";
        name += u.fName;
        name += " ";
        name += u.lName;
        return name;
    }

    return LPK;

}
