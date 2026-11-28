#include "User.h"

#include <json/json.h>
#include <unordered_map>

#include "Database/DbDoctor.h"
#include "Database/DbPractice.h"
#include "Model/Validators/LpkValidator.h"
#include "Resources.h"

static Practice s_practice;
static Doctor s_doctor;
static std::unordered_map<std::string, std::pair<double, double>> s_priceList;

std::unordered_map<std::string, std::string> s_names;

void User::initialize()
{
    s_names = DbDoctor::getPracticeAndDoctorNames();

	Json::Value root;
	Json::Reader reader;
	reader.parse(Resources::fromPath(":/json/json_bzs.json"), root);

    for (const auto& id : root.getMemberNames()) {
        s_names.emplace(id, root[id].asString());
    }
}


bool User::isValid()
{
    LpkValidator v;
    return v.validateInput(s_doctor.LPK);
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

SignatureTablet& User::signatureTablet()
{
   static SignatureTablet s_tablet;

   return s_tablet;
}


void User::setCurrentDoctor(const Doctor& doctor)
{
    s_doctor = doctor;
}

void User::setCurrentPractice(const Practice& practice)
{
    s_practice = practice;

    s_priceList = DbPractice::getCodeValues(practice.rziCode);

    Ekatte::s_unfavList = DbPractice::getUnfavEkatte(practice.rziCode);
}

void User::resetUser()
{
    s_practice = Practice();
    s_doctor = Doctor();
}


bool User::isCurrentUser(const std::string& LPK)
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

void User::addDoctorNames(const std::map<std::string, std::string>& names)
{
    for (const auto& [lpk, name] : names) {
        s_names.insert_or_assign(lpk, name);
    }
}

std::string User::getNameFromLPK(const std::string& LPK, bool prefix)
{
	std::string result = prefix ? doctorPrefix : "";

    if (s_names.count(LPK)) {
        result += s_names[LPK];
    }
    else {
		result = LPK;
    }

    return result;
}
/*
double User::getPrice(int procedureCode)
{
    for (auto& item : s_practice.priceList)
    {
        if (item.code == procedureCode)
            return item.price;
    }

    return 0;
}
*/
bool User::hasNhifContract()
{
    return s_practice.nhif_contract && s_doctor.specialty != NhifSpecialty::None;
}

bool User::isIncognito()
{
    return DbDoctor::isIncognito(s_doctor.LPK);
}

void User::setIncognito(bool incognito)
{
    DbDoctor::setIncognito(incognito, s_doctor.LPK);
}

std::pair<double, double> User::getPrice(const std::string &achiCode)
{
    if(!s_priceList.count(achiCode)) return {0, 0};

    return s_priceList[achiCode];
}
