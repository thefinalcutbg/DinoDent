#pragma once
#include <string>
#include "Model/Date.h"

class TiXmlDocument;
struct Practice;

struct NhifInvoiceData {
	
	NhifInvoiceData(const TiXmlDocument& monthNotif);

	const std::string fin_document_type_code; //INVOICE, CT_NOTIF, DT_NOTIF
	const int fin_document_month_no; //month notif number
	const int activityTypeCode;					//nhif_type_code
	std::string health_insurance_fund_type_code;	//NZOK, MZ etc...

	const Date date_from;
	const Date date_to;

	std::string monthNotifData; //the whole monthly notification data

};