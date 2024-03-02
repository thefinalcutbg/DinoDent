#include "NhifInvoiceData.h"
#include <TinyXML/tinyxml.h>
#include "Model/UserStructs.h"

const char* getText1(const TiXmlElement* element)
{
    if (element == nullptr) {
        throw std::exception();
    }

    return element->GetText();
}

std::string getMonthNotifData(const TiXmlDocument& monthNotif)
{
    TiXmlPrinter printer;

    monthNotif.Accept(&printer);

    return printer.Str();
}

NhifInvoiceData::NhifInvoiceData(const TiXmlDocument& monthNotif)
	:
    fin_document_month_no(std::stoi(getText1(monthNotif.RootElement()->FirstChildElement("monthly_notification_num")))),
    activityTypeCode(std::stoi(getText1(monthNotif.RootElement()->FirstChildElement("nhif_type_code")))),
    date_from {(getText1(monthNotif.RootElement()->FirstChildElement("date_from")))},
    date_to{(getText1(monthNotif.RootElement()->FirstChildElement("date_to"))) },
    monthNotifData{ getMonthNotifData(monthNotif) }
{
}

std::string NhifInvoiceData::HIFTypeCode() const
{
    constexpr const char* insuranceFunds[26]
    {
    "",
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
    "NZOK"
    };

    if (activityTypeCode < 0 || activityTypeCode > 25) return std::string();

    return insuranceFunds[activityTypeCode];
}
