#include "Invoice.h"

#include "Model/User.h"
#include <TinyXML/tinyxml.h>
#include "Model/FreeFunctions.h"
#include "BusinessOperation.h"
#include "Model/User.h"


std::string getText(const TiXmlElement* element)
{
        if (element == nullptr){
            throw std::exception();
    }

    return element->GetText();
}

FinancialDocType getFinancialType(const std::string& inv_type_code)
{

	if (inv_type_code == "INVOICE") return FinancialDocType::Invoice;

	if (inv_type_code == "DT_NOTIF") return FinancialDocType::Debit;

	if (inv_type_code == "CT_NOTIF") return FinancialDocType::Credit;

    throw std::exception();
}


Invoice::Invoice(const TiXmlDocument& monthNotif, const std::string& claimedHash, const Practice& practice, const Doctor& doctor)
    :
    type{ getFinancialType(getText(monthNotif.RootElement()->FirstChildElement("inv_type_code")))},

    nhifData{ NhifInvoiceData(monthNotif)},

    recipient{ practice },
    m_mainDocument{
        type == FinancialDocType::Invoice
            ?
            MainDocument{}
            :
            MainDocument{
                std::stoll(getText(monthNotif.RootElement()->FirstChildElement("from_inv_num"))),
                Date(getText(monthNotif.RootElement()->FirstChildElement("from_inv_date")))
            }
    }
{

	nhifData->claimedHash = claimedHash;

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

	paymentType = PaymentType::Bank;
    taxEventDate = nhifData.value().date_to;


}

Invoice::Invoice(const Patient& p) :
    type(FinancialDocType::Invoice),
	recipient(p)
	
{
}

std::string Invoice::nhifDocumentTypeCode() const
{
	const char* fin_document_type_code[] = { "INVOICE", "DT_NOTIF", "CT_NOTIF" };

	return fin_document_type_code[static_cast<int>(type)];
}


Invoice::Invoice(const Recipient& r) :
    type (FinancialDocType::Invoice),
	recipient(r)
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

std::string Invoice::title() const
{
	const char* titles[] = { "Фактура", "Дебитно известие", "Кредитно известие" };

	return titles[static_cast<int>(type)];
}

Issuer Invoice::issuer() const
{
	return Issuer();
}

std::string Invoice::getInvoiceNumber() const
{
	return FreeFn::leadZeroes(number, 10);
}

void Invoice::removeOperation(int idx)
{
	businessOperations.erase(businessOperations.begin() + idx);
}

void Invoice::addOperation(const BusinessOperation& op)
{
	businessOperations.push_back(op);
}

void Invoice::editOperation(const BusinessOperation& op, int idx)
{
	businessOperations[idx] = op;
}

double Invoice::amount() const
{
	double result = 0;

	for (auto& operation : businessOperations)
	{
		result += operation.value_price;
	}

    return result;
}

double Invoice::VATamount() const
{
    if(!isVAT) return 0;

    return amount()*0.2;
}

std::string Invoice::getFileName() const //only nhif data files can be exported as xml
{
	if (!nhifData.has_value()) return std::string{};

	return
		//"FILE_SUBM_FDOC_" +
		nhifDocumentTypeCode() + "_" +
		User::practice().rziCode + "_" +
		std::to_string(nhifData->activityTypeCode) + "_" +
		taxEventDate.toXMLInvoiceFileName() + "_" +
		FreeFn::leadZeroes(nhifData->fin_document_month_no, 10)
		+ ".xml";

}
