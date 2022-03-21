#pragma once
#include <string>
#include "Model/Financial/Invoice.h"

class IInvoiceDialog;

class InvoicePresenter
{

	IInvoiceDialog* view{nullptr};

	std::optional<Invoice> m_invoice;

	std::string outputFileName;

	

public:
	void loadFile(const std::string& filePath);
	void saveAsXML(const std::string& filePath);
	void setView(IInvoiceDialog* view) { this->view = view; };
	void docNumberChanged(int number);
	void docDateChanged(Date date);
	void open();
	bool invoiceIsValid();

};

