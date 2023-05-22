#pragma once
#include "Model/Patient.h"

class ToothContainer;

class IProcedureHistoryDialog {

public:
	virtual void setPis(const std::vector<Procedure>& p) = 0;
	virtual void setHis(const std::vector<Procedure>& h) = 0;
	virtual void closeDialog() = 0;
	virtual void setCurrentStatus(const ToothContainer& teeth) = 0;
};