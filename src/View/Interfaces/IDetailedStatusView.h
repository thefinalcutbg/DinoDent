#pragma once

class CheckModel;
struct ToothPaintHint;
class DetailedStatusPresenter;


#include "Model/Dental/Procedure.h"
#include <vector>

class IDetailedStatusView
{

public:

	virtual void setHistoryData(const std::vector<Procedure>& history) = 0;
	virtual void setNotes(const std::string& notes) = 0;

	virtual std::string getNotes() = 0;


};
