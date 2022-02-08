#pragma once
#include <string>
#include <string_view>
#include "Model/Date.h"

struct InvoiceRecipient
{
	std::string recipient_code;
	std::string recipient_name;
	std::string recipient_address;
	std::string recipient_bulstat;
};

const InvoiceRecipient& getRecipient(const std::string& RHIF);
std::string getInsuranceFund(int activityTypeCode);

struct InvoiceDocumentXML
{
	std::string fin_document_type_code; //DT_NOTIF, CT_NOTIF
	std::string fin_document_no; //leading zeroes! input by user
	std::string fin_document_month_no; //monthly_notification_num
	std::string fin_document_date; //input by user
	std::string document_no; //from_inv_num 
	std::string document_date; //from_inv_date

	int activityTypeCode;
	std::string health_insurance_fund_type_code;
	std::string date_from;
	std::string date_to;

};