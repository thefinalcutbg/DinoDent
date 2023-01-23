#pragma once

class CheckModel;
class ToothPaintHint;

struct ImplantData;
struct DentistData;
struct CrownData;
struct Pathology;
struct ObturationData;
struct DetailedStatusPresenter;


#include "Model/Dental/Procedure.h"
#include <vector>

class IDetailedStatusView
{

public:

	virtual void setHistoryData(const std::vector<Procedure>& history) = 0;
	virtual void setCheckModel(const CheckModel& model) = 0;
	virtual void disableItem(int index, bool disabled) = 0;
	virtual void paintTooth(const ToothPaintHint& hint) = 0;

	virtual void setData(const std::string& notesData) = 0;

	virtual std::string getNotes() = 0;


};