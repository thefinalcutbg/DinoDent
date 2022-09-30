#pragma once

#include <vector>
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/IPatientTileInfo.h"
class CheckModel;
class AmbList;
class Patient;
class ToothPaintHint;
class ISurfacePanel;
class ListPresenter;
class AbstractComboBox;
struct NhifSheetData;
class Date;
class Time;

class IListView
{
public:
	virtual void refresh(const AmbList& ambList) = 0;
	virtual void setPresenter(ListPresenter* presenter) = 0;
	virtual void setAmbListNum(int number) = 0;
	virtual void setCheckModel(const CheckModel& checkModel) = 0;
	virtual void setSelectedTeeth(std::vector<int> selectedTeeth) = 0;
	virtual void refreshPriceLabel(double patientPrice, double nzokPrice) = 0;
	virtual void setDateTime(const Date& date, const Time& time) = 0;
	virtual void hideSurfacePanel(bool hidden) = 0;
	virtual void hideControlPanel(bool hidden) = 0;
	virtual void hideNhifSheetData() = 0;
	virtual void setNhifData(const NhifSheetData& data) = 0;
	virtual ISurfacePanel* surfacePanel() = 0;
	virtual IPatientTileInfo* tileInfo() = 0;
	virtual void repaintTooth(const ToothPaintHint& tooth) = 0;
	virtual void setNotes(const std::array<std::string, 32>& notes) = 0;

	virtual void setProcedures(const std::vector<Procedure>& m) = 0;

	virtual void disableGraphicsView(bool disabled) = 0;
};