#pragma once

class FiscalReceiptDialog;

class FiscalReceiptPresenter
{
	long long amblist_rowid;

	FiscalReceiptDialog* view{ nullptr };

public:

	FiscalReceiptPresenter(long long ambRowid) : amblist_rowid(ambRowid) {};

	void openDialog();

	void okPressed();

	void setView(FiscalReceiptDialog* v);

};
