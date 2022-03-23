#pragma once
#include <string>
#include "Model/Date.h"

class TiXmlDocument;
struct Practice;

struct NZOKInvoiceData {
	
	NZOKInvoiceData(const TiXmlDocument& monthNotif, const Practice& practice);

	std::string NzokRecipientCode; //nzok rhif

	const std::string fin_document_month_no; //month notif number
	const int activityTypeCode;					//nhif_type_code
	std::string health_insurance_fund_type_code;	//NZOK, MZ etc...

	const std::string contract_no; //nzok contract
	const Date contract_date;	//nzok contract date
	const std::string rhi_nhif_no; //practice rzi number

	const Date date_from;
	const Date date_to;

	std::string outputFileName;

};