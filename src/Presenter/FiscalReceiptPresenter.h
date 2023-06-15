#pragma once

#include "Model/Financial/FiscalReceipt.h"
#include "View/Interfaces/IFiscalReceiptDialog.h"

class FiscalReceiptPresenter
{
	long long amblist_rowid;

	IFiscalReceiptDialog* view{ nullptr };

public:

	FiscalReceiptPresenter(long long ambRowid) : amblist_rowid(ambRowid) {};

	void openDialog();

	void okPressed();

	void setView(IFiscalReceiptDialog* v);

};
