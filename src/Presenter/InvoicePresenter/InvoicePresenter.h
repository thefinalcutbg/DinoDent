#pragma once
#include <string>
#include "Model/XML/InvoiceXML.h"

class IInvoiceDialog;

class InvoicePresenter
{

	IInvoiceDialog* view{nullptr};

	std::optional<Invoice> m_invoice;

	bool invoiceIsValid();

public:
	void loadFile(const std::string& filePath);
	void saveAsXML(const std::string& filePath);
	void setView(IInvoiceDialog* view) { this->view = view; };
	void open();

};

