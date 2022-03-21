#include "NzokInvoiceData.h"
#include "Libraries/TinyXML/tinyxml.h"
#include "Model/User/User.h"

constexpr const char* insuranceFunds[46]
{
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"NZOK",
"NZOK",
"NZOK",
"MZ",
"MZ",
"MZ",
"NZOK",
"NZOK",
"MZ",
"MZ",
"NZOK",
"NZOK",
"MZ",
"NZOK",
"NZOK",
"MZ",
"ASP",
"ASP",
"NZOK",
"NZOK",
"MZ",
"MZ",
"MZ",
"MZ",
"MZ",
"NZOK",
"NZOK",
"NZOK",
"MZ",
"NZOK",
"ASP",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ",
"NZOK",
"MZ"
};

const char* getText1(const TiXmlElement* element)
{
    if (element == nullptr) {
        throw std::exception("wrong xml format");
    }

    return element->GetText();
}


NZOKInvoiceData::NZOKInvoiceData(const TiXmlDocument& monthNotif, const Practice& practice)
	:
    rhi_nhif_no(practice.rziCode),
    contract_date(practice.nzok_contract.value().date),
    contract_no(practice.nzok_contract.value().contract_no),
	NzokRecipientCode(practice.RHIF()),
	activityTypeCode(std::stoi(getText1(monthNotif.RootElement()->FirstChildElement("nhif_type_code")))),
    health_insurance_fund_type_code (insuranceFunds[activityTypeCode-1]),
    fin_document_month_no(getText1(monthNotif.RootElement()->FirstChildElement("monthly_notification_num"))),
    date_from {Date::getDateFromXmlFormat(getText1(monthNotif.RootElement()->FirstChildElement("date_from")))},
    date_to{ Date::getDateFromXmlFormat(getText1(monthNotif.RootElement()->FirstChildElement("date_to"))) }

{
}
