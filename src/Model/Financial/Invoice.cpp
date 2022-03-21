#include "Invoice.h"

#include <array>
#include <fstream>
#include "Model/User/User.h"
#include <stdexcept>
#include "Libraries/TinyXML/tinyxml.h"


const char* getText(const TiXmlElement* element)
{
        if (element == nullptr){
            throw std::exception("wrong xml format");
    }

    return element->GetText();
}


std::string getDocumentName(const TiXmlElement* element)
{
   std::string s = getText(element->FirstChildElement("inv_type"));

   s.erase(0, 40);
   s += u8" за ";
   s += getText(element->FirstChildElement("nhif_type"));

   return s;
}

FinancialDocType getType(const std::string& inv_type_code)
{
	if (inv_type_code == "INVOICE") return FinancialDocType::Invoice;

	if (inv_type_code == "CT_NOTIF") return FinancialDocType::Credit;

	if (inv_type_code == "DT_NOTIF") return FinancialDocType::Debit;
}

Invoice::Invoice(const TiXmlDocument& monthNotif, const User& user)
    :
    name                        {getDocumentName(monthNotif.RootElement())},
	type						{getType(getText(monthNotif.RootElement()->FirstChildElement("inv_type_code")))},
	
	
    mainDocument{           
        type == FinancialDocType::Invoice
        ?
        std::optional<MainDocument>{} 
        :
        MainDocument{
        getText(monthNotif.RootElement()->FirstChildElement("from_inv_num")),
        Date::getDateFromXmlFormat(getText(monthNotif.RootElement()->FirstChildElement("from_inv_date")))
        }
    },

	nzokData{ NZOKInvoiceData(monthNotif, user.practice)},
	recipient						{stoi(user.practice.RHIF())},
	issuer							{user}
{

	for (
		const TiXmlElement* business_operation = monthNotif.RootElement()->FirstChildElement("Monthly_Notification_Details");
		business_operation != NULL;
		business_operation = business_operation->NextSiblingElement("Monthly_Notification_Details")
		)
	{
		businessOperations.emplace_back(

			BusinessOperation{
				business_operation->FirstChildElement("activity_code")->GetText(),
				business_operation->FirstChildElement("activity_name")->GetText(),
				std::stof(business_operation->FirstChildElement("unit_price")->GetText()),
                std::stoi(business_operation->FirstChildElement("quantity")->GetText()),
				std::stof(business_operation->FirstChildElement("value_price")->GetText())
			}
		);
	}

    for (auto& operation : businessOperations)
    {
        aggragated_amounts.payment_amount += operation.value_price;
    }

    aggragated_amounts.total_amount = aggragated_amounts.payment_amount;
    aggragated_amounts.taxEventDate = nzokData.value().date_to;


}



