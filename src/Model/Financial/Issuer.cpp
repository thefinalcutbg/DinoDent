#include "Issuer.h"
#include "Model/User/User.h"

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


Issuer::Issuer() : Issuer(User::practice(), User::doctor())
{
}

Issuer::Issuer(const Practice& practice, const Doctor& doctor) :
    type{ getIssuerType(practice.legal_entity, doctor) },
    company_name{ 
        practice.nzok_contract.has_value() ?
        practice.nzok_contract.value().name_short 
        :
        practice.name

    },
    address_by_contract{ practice.firm_address },
    address_by_activity{ practice.practice_address },

    registration_by_VAT{

                practice.vat.empty() ?

                std::optional<std::string>{}
                :
                practice.vat
    },

    grounds_for_not_charging_VAT{
                                                
                    registration_by_VAT ?
                                                
                    u8"Чл. 39 от ЗДДС"
                    :
                    u8"Чл.113,ал.9 от ЗДДС"
    },

    bulstat{ practice.bulstat }
{}
