#pragma once
#include "Model/Patient.h"
#include "Model/Hospitalization.h"

struct HisSnapshot;

class IProcedureHistoryDialog {

public:

	virtual void setPis(const std::vector<Procedure>& p) = 0;
	virtual void setHis(const std::vector<Procedure>& h) = 0;
	virtual void setHospitalizations(const std::vector<Hospitalization>& h) = 0;
	virtual void closeDialog() = 0;
	virtual void setSliderIndex(int index) = 0;
	virtual void setSliderCount(int count) = 0;
	virtual void setSnapshot(const HisSnapshot& snapshot) = 0;
	virtual void focusTab(int idx) = 0;
};