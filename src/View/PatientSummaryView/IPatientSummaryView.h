#pragma once

#include <vector>
#include "Model/Procedure/Procedure.h"
#include "Model/Date.h"

class PatientSummaryPresenter;
class ToothPaintHint;
class Patient;

class IPatientSummaryView
{
public:
	virtual void setPresenter(PatientSummaryPresenter* presenter) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual void setAmbDates(const std::vector<Date>& ambDates) = 0;
	virtual void setTeeth(const ToothPaintHint& tooth) = 0;
	virtual void setProcedures(const std::vector<Procedure>& m) = 0;


};