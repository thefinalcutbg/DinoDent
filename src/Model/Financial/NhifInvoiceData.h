#pragma once
#include <string>
#include "Model/Date.h"

class TiXmlDocument;

struct NhifInvoiceData {
	
	NhifInvoiceData(const TiXmlDocument& monthNotif);

	const int fin_document_month_no; //month notif number
	const int activityTypeCode;		//nhif_type_code
	std::string HIFTypeCode() const; //NZOK, MZ etc...
	
	const Date date_from;
	const Date date_to;

	std::string monthNotifData; //the whole monthly notification data
};