#pragma once

#include <vector>
#include <array>
#include "Model/Procedure/Procedure.h"
#include "Model/Date.h"
#include "View/ToothPaintDevices/PaintHint.h"


class PatientSummaryPresenter;
struct PerioWithDisabled;
struct Patient;
class PerioStatistic;

class IPatientSummaryView
{
public:
	virtual void setPresenter(PatientSummaryPresenter* presenter) = 0;
	virtual void setTimeFrameCount(int count) = 0;
	virtual void setTickPosition(int idx) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual void setTeeth(const std::array<ToothPaintHint, 32>& teeth) = 0;
	virtual void setProcedures(const std::vector<Procedure>& p) = 0;
	virtual void setPerioData(const PerioWithDisabled& perio) = 0;
	virtual void setInitialAmbList() = 0;
	virtual void setPerioStatistic(const PerioStatistic& stat) = 0;
	virtual void setDocumentLabel(const std::string& label, const std::string& date, const std::string& doctor) = 0;
	virtual void setToothInfo(const std::string& info) = 0;
	virtual void setSelectedTooth(int toothIdx) = 0;

};