#pragma once

#include <vector>
#include "Model/Procedure/Procedure.h"

class CheckModel;
class AmbList;
class Patient;
class ToothPaintHint;
class ISurfacePanel;
class ListPresenter;
class AbstractComboBox;

class IListView
{
public:
	virtual void refresh(const AmbList& ambList, const Patient &patient) = 0;
	virtual void setPresenter(ListPresenter* presenter) = 0;
	virtual void setAmbListNum(int number) = 0;
	virtual void setCheckModel(const CheckModel& checkModel) = 0;
	virtual void setSelectedTeeth(std::vector<int> selectedTeeth) = 0;

	virtual void hideSurfacePanel(bool hidden) = 0;
	virtual void hideControlPanel(bool hidden) = 0;
	virtual ISurfacePanel* surfacePanel() = 0;

	virtual void repaintTooth(const ToothPaintHint& tooth) = 0;
	virtual void setNotes(const std::array<std::string, 32>& notes) = 0;

	virtual void setProcedures(const std::vector<Procedure>& m, double patientPrice, double nzokPrice) = 0;
	virtual void setUnfav(bool unfav) = 0;
	virtual AbstractComboBox* taxCombo() = 0;

	virtual void disableGraphicsView(bool disabled) = 0;
};