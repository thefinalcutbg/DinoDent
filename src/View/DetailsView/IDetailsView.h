#pragma once
#include "SubWidgets/IDetailedStatusView.h"
class CheckModel;

class IDetailsView
{
public:
	virtual IDetailedStatusView* detailedStatus() = 0;
};