#pragma once
#include "SubWidgets/IDetailedStatusView.h"
#include "Model/Procedure/TableStructs.h"
#include <vector>
class CheckModel;

class IDetailsView
{
public:
	virtual IDetailedStatusView* detailedStatus() = 0;
	virtual void setHistoryData(const std::vector<DetailsSummary> &history) = 0;
};