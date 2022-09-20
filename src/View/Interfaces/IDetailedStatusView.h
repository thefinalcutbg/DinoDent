#pragma once

class CheckModel;
class ToothPaintHint;

class ImplantData;
class DentistData;
class CrownData;
class PathologyData;
class ObturationData;
class DetailedStatusPresenter;

#include "Model/Dental/Procedure.h"
#include <vector>

class IDetailedStatusView
{

public:

	virtual void setHistoryData(const std::vector<Procedure>& history) = 0;
	virtual void setCheckModel(const CheckModel& model) = 0;
	virtual void disableItem(int index, bool disabled) = 0;
	virtual void paintTooth(const ToothPaintHint& hint) = 0;

	virtual void clearData() = 0;

	virtual void setData(const ImplantData& data) = 0;
	virtual void setData(const ObturationData& data) = 0;
	virtual void setData(const DentistData& data) = 0;
	virtual void setData(const CrownData& data) = 0;
	virtual void setData(const PathologyData& data) = 0;
	virtual void setData(const std::string& notesData) = 0;

	virtual void disableDetails(bool disabled) = 0;
	virtual ObturationData getObturationData() = 0;
	virtual ImplantData getImplantData() = 0;
	virtual bool getDentistData() = 0;
	virtual CrownData getCrownData() = 0;
	virtual int getDiagnosisIndex() = 0;
	virtual std::string getNotes() = 0;

};