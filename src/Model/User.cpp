#include "User.h"
#include "Database/DbDoctor.h"


static Practice s_practice;
static Doctor s_doctor;

std::unordered_map<std::string, std::string> s_names;

void User::initialize()
{
    s_names = DbDoctor::getDoctorNames();
}


const Practice& User::practice()
{
    return s_practice;
}

const Settings& User::settings()
{
    return s_practice.settings;
}

const Doctor& User::doctor()
{
    return s_doctor;
}

void User::setCurrentDoctor(const Doctor& doctor)
{
    s_doctor = doctor;
}

void User::setPriceList(const std::vector<ProcedureTemplate>& priceList)
{
    s_practice.priceList = priceList;
}

void User::setCurrentPractice(const Practice& practice)
{
    s_practice = practice;
}

void User::resetUser()
{
    s_practice = Practice();
    s_doctor = Doctor();
}


const bool User::isCurrentUser(const std::string& LPK)
{
    return LPK == s_doctor.LPK;
}

bool User::isAdmin()
{
    return std::get<0>(DbDoctor::getAdminAndSpecialty(s_doctor.LPK, s_practice.rziCode));
}

void User::refereshPracticeDoctor()
{
   s_doctor.specialty = static_cast<NhifSpecialty>(std::get<1>(DbDoctor::getAdminAndSpecialty(s_doctor.LPK, s_practice.rziCode)));
}

std::string User::getNameFromLPK(const std::string& LPK)
{
    if (s_names.count(LPK)) return s_names[LPK];

    return LPK;

}

double User::getPrice(int procedureCode)
{
    for (auto& item : s_practice.priceList)
    {
        if (item.code == procedureCode)
            return item.price;
    }

    return 0;
}

bool User::hasNzokContract()
{
    return s_practice.nzok_contract && s_doctor.specialty != NhifSpecialty::None;
}
