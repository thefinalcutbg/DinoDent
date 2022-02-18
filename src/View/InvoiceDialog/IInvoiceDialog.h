#pragma once
#include <string>
struct Invoice;
class Date;

class IInvoiceDialog
{
public:
	virtual void enableUserFields() = 0;
	virtual void setInvoice(const Invoice& inv) = 0;
	virtual void refreshNumberAndDate(const std::string& number, const Date& date) = 0;
};