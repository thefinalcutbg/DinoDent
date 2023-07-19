#include "Invoice.h"

#include <array>
#include <fstream>
#include "Model/User.h"
#include <stdexcept>
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "BusinessOperation.h"



std::string getText(const TiXmlElement* element)
{
        if (element == nullptr){
            throw std::exception("Неуспешно зареждане на месечното известие");
    }

    return element->GetText();
}


std::string getDocumentName(const TiXmlElement* element)
{
   std::string s = getText(element->FirstChildElement("inv_type"));

   s.erase(0, 40);
   s += " за ";
   s += getText(element->FirstChildElement("nhif_type"));

   return s;
}

FinancialDocType getFinancialType(const std::string& inv_type_code)
{

	if (inv_type_code == "INVOICE") return FinancialDocType::Invoice;

	if (inv_type_code == "DT_NOTIF") return FinancialDocType::Debit;

	if (inv_type_code == "CT_NOTIF") return FinancialDocType::Credit;

	throw std::exception("Неразпознат inv_type_code");
}


Invoice::Invoice(const TiXmlDocument& monthNotif, const Practice& practice, const Doctor& doctor)
    :
    name                        {getDocumentName(monthNotif.RootElement())},
	type						{ getFinancialType(getText(monthNotif.RootElement()->FirstChildElement("inv_type_code")))},
	
	
    m_mainDocument{           
        type == FinancialDocType::Invoice
        ?
		MainDocument{}
        :
        MainDocument{
        std::stoll(getText(monthNotif.RootElement()->FirstChildElement("from_inv_num"))),
        Date(getText(monthNotif.RootElement()->FirstChildElement("from_inv_date")))
        }
    },

	nhifData{ NhifInvoiceData(monthNotif, practice)},
	recipient						{std::stoi(practice.RHIF())},
	issuer							{practice, doctor}
{

	for (
		const TiXmlElement* business_operation = monthNotif.RootElement()->FirstChildElement("Monthly_Notification_Details");
		business_operation != NULL;
		business_operation = business_operation->NextSiblingElement("Monthly_Notification_Details")
		)
	{
		businessOperations.emplace_back();

		auto& bOp = businessOperations.back();

		bOp.activity_code = getText(business_operation->FirstChildElement("activity_code"));
		bOp.activity_name = getText(business_operation->FirstChildElement("activity_name"));
		bOp.unit_price = std::stof(getText(business_operation->FirstChildElement("unit_price")));
		bOp.quantity = std::stoi(getText(business_operation->FirstChildElement("quantity")));
		bOp.value_price = std::stof(getText(business_operation->FirstChildElement("value_price")));
			
	
	}

	aggragated_amounts.calculate(businessOperations);
	aggragated_amounts.paymentType = PaymentType::Bank;
    aggragated_amounts.taxEventDate = nhifData.value().date_to;


}

Invoice::Invoice(const Patient& p, const Practice& practice, const Doctor& doctor) :
	name ("Фактура"),
	type(FinancialDocType::Invoice),
	recipient(p),
	issuer{practice, doctor}
{
}


Invoice::Invoice(const Recipient& r, const Practice& p, const Doctor& d) :
	name ("Фактура"),
	type (FinancialDocType::Invoice),
	recipient(r),
	issuer{ p, d }
{
}

std::optional<MainDocument> Invoice::mainDocument() const
{
	if (type == FinancialDocType::Invoice)	return {};

	return m_mainDocument;
}

void Invoice::setMainDocumentData(long long num, Date date)
{
	m_mainDocument.number = num;
	m_mainDocument.date = date;
}

std::string Invoice::getInvoiceNumber() const
{
	return FreeFn::leadZeroes(number, 10);
}

void Invoice::removeOperation(int idx)
{
	businessOperations.erase(businessOperations.begin() + idx);
	aggragated_amounts.calculate(businessOperations);
}

void Invoice::addOperation(const BusinessOperation& op)
{
	businessOperations.push_back(op);
	aggragated_amounts.calculate(businessOperations);
}

void Invoice::editOperation(const BusinessOperation& op, int idx)
{
	businessOperations[idx] = op;
	aggragated_amounts.calculate(businessOperations);
}

std::string Invoice::getFileName() //only nhif data files can be exported as xml
{
	if (!nhifData.has_value()) return std::string{};

	return
		//"FILE_SUBM_FDOC_" +
		nhifData->fin_document_type_code + "_" +
		User::practice().rziCode + "_" +
		std::to_string(nhifData->activityTypeCode) + "_" +
		aggragated_amounts.taxEventDate.toXMLInvoiceFileName() + "_" +
		FreeFn::leadZeroes(nhifData->fin_document_month_no, 10)
		+ ".xml";

}
/*
Invoice& Invoice::operator=(const Invoice& other)
{
	rowId = other.rowId;
	type = other.type;
	number = other.number;
	name = other.name;
	date = other.date;
	m_mainDocument = other.m_mainDocument;
	nzokData.emplace(other.nzokData);
	recipient = other.recipient;
	issuer = other.issuer;
	businessOperations = other.businessOperations;
	aggragated_amounts = other.aggragated_amounts;
	
	return *this;
}
*/

void AggregatedAmounts::calculate(const BusinessOperations& operations)
{
	payment_amount = 0;

	for (auto& operation : operations)
	{
		payment_amount += operation.value_price;
	}
	
	total_amount = payment_amount;
}
