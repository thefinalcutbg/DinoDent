#include "UserManager.h"
#include "Database/DbDoctor.h"

static User s_currentUser;

std::unordered_map<std::string, std::string> s_names;



void UserManager::initialize()
{
    s_names = DbDoctor::getDoctorNames();
}

const User& UserManager::currentUser()
{
    return s_currentUser;
}

const Practice& UserManager::practice()
{
    return s_currentUser.practice;
}

Doctor& UserManager::doctor()
{
    return s_currentUser.doctor;
}

void UserManager::setCurrentUser(const User& user)
{
    s_currentUser = user;
}

void UserManager::setCurrentDoctor(const Doctor& doctor)
{
    s_currentUser.doctor = doctor;
}

void UserManager::setPriceList(const std::vector<ProcedureTemplate>& priceList)
{
    s_currentUser.practice.priceList = priceList;
}

void UserManager::setCurrentPractice(const Practice& practice)
{
    s_currentUser.practice = practice;
}

void UserManager::resetUser()
{
    s_currentUser = User();
}


const bool UserManager::isCurrentUser(const std::string& LPK)
{
    return LPK == s_currentUser.doctor.LPK;
}

std::string UserManager::getDoctorName(const std::string& LPK)
{
    if (s_names.count(LPK))
        return s_names[LPK];

    return LPK;

}
