#pragma once
#include <vector>
#include "Model/Financial/FiscalReceipt.h"

class IFiscalReportView
{
public:
	virtual void setFiscalData(const std::vector<FiscalReceipt>& data) = 0;
};