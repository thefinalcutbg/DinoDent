#include "Issuer.h"
#include "Model/User.h"

IssuerType getIssuerType(int legalEntity, const Doctor& doctor)
{
    const char* legalForms[5]{
        "ЕТ","ЕООД","ООД","ЕАД","АД"
    };

    switch (legalEntity)
    {
    case 0:
        return SelfInsured{
            "Декларация по чл.43,ал.5 от ЗДДФЛ, че съм самоосигуряващо се лице по смисъла на КСО",
            PersonInfo{
                User::practice().selfInsuredId,
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

std::optional<std::string> Issuer::vat() const
{
     if (!m_vat) return {};

     return "BG" + bulstat;
}

Issuer::Issuer(const Practice& practice, const Doctor& doctor) :
    m_vat(practice.hasVat),
    type{ getIssuerType(practice.legal_entity, doctor) },
    company_name{
        practice.nhif_contract.has_value() ?
        practice.nhif_contract.value().name_short
        :
        practice.name

    },
    address_by_contract{ practice.firm_address },
    address_by_activity{ practice.addresssByActivity() },

    grounds_for_not_charging_VAT{

                    m_vat ?

                    "Чл. 39 от ЗДДС"
                    :
                    "Чл.113,ал.9 от ЗДДС"
    },

    bulstat{ practice.bulstat },
    bank(practice.bank),
    iban(practice.iban),
    bic(practice.bic)
{}
