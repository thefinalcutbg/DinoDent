#pragma once
#include "Model/Financial/FiscalReceipt.h"

class IFiscalReceiptDialog
{
public:
    virtual void setReceipt(const std::string& timestamp, const std::string& fiscalMemory) = 0;
	virtual FiscalReceipt getReceipt() = 0;
	virtual void closeDialog() = 0;
};
