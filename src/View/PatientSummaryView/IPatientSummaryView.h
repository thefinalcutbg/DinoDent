#pragma once

#include <vector>
#include <array>
#include "Model/Procedure/Procedure.h"
#include "Model/Date.h"
#include "View/ToothPaintDevices/PaintHint.h"

class PatientSummaryPresenter;
class ToothPaintHint;
class Patient;

class IPatientSummaryView
{
public:
	virtual void setDateLabel(const std::string& dateLabel) = 0;
	virtual void setPresenter(PatientSummaryPresenter* presenter) = 0;
	virtual void setTimeFrameCount(int count) = 0;
	virtual void setTickPosition(int idx) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual void setTeeth(const std::array<ToothPaintHint, 32>& teeth) = 0;
	virtual void setProcedures(const std::vector<Procedure>& p) = 0;


};