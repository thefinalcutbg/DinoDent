#include "UserManager.h"

UserManager UserManager::m_instance;
User UserManager::m_currentUser;

#include "Database/DbLogin.h"

void UserManager::initialize()
{
    DbLogin db;
    m_names = db.getDoctorNames();
}

UserManager::UserManager()
{

}

const bool UserManager::isCurrentUser(const std::string& LPK)
{
    return LPK == m_currentUser.LPK;
}

std::string UserManager::getDoctorName(const std::string& LPK)
{
    if(m_names.count(LPK))
    return m_names[LPK];

    return LPK;

}
