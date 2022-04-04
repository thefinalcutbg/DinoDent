#include "Issuer.h"
#include "Model/User/UserManager.h"

IssuerType getIssuerType(int legalEntity, const Doctor& doctor)
{
    const char* legalForms[5]{
        u8"ET",u8"ЕООД",u8"ООД",u8"ЕАД",u8"АД"
    };

    switch (legalEntity)
    {
    case 0:
        return SelfInsured{
            u8"Декларация по чл.43,ал.5 от ЗДДФЛ, че съм самоосигуряващо се лице по смисъла на КСО",
            PersonInfo{
                doctor.egn,
                doctor.fname,
                doctor.mname,
                doctor.lname
            }
        };
    default:
        return Company{ legalForms[legalEntity - 1] };
    }
}


Issuer::Issuer() : Issuer(UserManager::currentUser())
{
}

Issuer::Issuer(const User& user) :
    type{ getIssuerType(user.practice.legal_entity, user.doctor) },
    company_name{ 
        user.practice.nzok_contract.has_value() ?
        user.practice.nzok_contract.value().name_short 
        :
        user.practice.name

    },
    address_by_contract{ user.practice.firm_address },
    address_by_activity{ user.practice.practice_address },

    registration_by_VAT{

                user.practice.vat.empty() ?

                std::optional<std::string>{}
                :
                user.practice.vat
    },

    grounds_for_not_charging_VAT{
                                                
                    registration_by_VAT ?
                                                
                    u8"Чл. 39 от ЗДДС"
                    :
                    u8"Чл.113,ал.9 от ЗДДС"
    },

    bulstat{ user.practice.bulstat }
{}
