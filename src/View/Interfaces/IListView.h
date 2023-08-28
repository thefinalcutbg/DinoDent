#pragma once

#include <vector>
#include "Model/Dental/Procedure.h"
#include "View/Interfaces/IPatientTileInfo.h"
#include "Model/Referrals/Referral.h"

class CheckModel;
class AmbList;
class Patient;
class ToothPaintHint;
class ISurfacePanel;
class ListPresenter;
class AbstractComboBox;
struct NhifSheetData;
struct MedicalNotice;
class Date;
class Time;

class IListView
{
public:

	struct HisButtonProperties {
		bool hideSpinBox{ false };
		const std::string labelText;
		const std::string buttonText;
		const std::string hoverText;
	};

	virtual void setPresenter(ListPresenter* presenter) = 0;
	virtual void setAmbListNum(int number) = 0;
	virtual void setCheckModel(const CheckModel& checkModel, const CheckModel& dsnCheckModel) = 0;
	virtual void setSelectedTeeth(std::vector<int> selectedTeeth) = 0;
	virtual void setDateTime(const std::string& time8601) = 0;
	virtual void hideSurfacePanel(bool hidden) = 0;
	virtual void hideControlPanel(bool hidden) = 0;
	virtual void hideNhifSheetData() = 0;
	virtual void setNhifData(const NhifSheetData& data, bool showUnfav) = 0;
	virtual ISurfacePanel* surfacePanel() = 0;
	virtual IPatientTileInfo* tileInfo() = 0;
	virtual void repaintTooth(const ToothPaintHint& tooth) = 0;
	virtual void setNotes(const std::array<std::string, 32>& notes) = 0;
	virtual void setAdditionalDocuments(const std::vector<Referral>& referrals, const std::vector<MedicalNotice>& notices) = 0;
	virtual void setProcedures(const std::vector<Procedure>& m) = 0;
	virtual void setHisButtonText(const HisButtonProperties& prop) = 0;

};