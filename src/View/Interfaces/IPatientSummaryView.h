#pragma once

#include <vector>
#include <array>
#include "Model/Dental/Procedure.h"
#include "Model/Date.h"
#include "View/Graphics/PaintHint.h"


class PatientSummaryPresenter;
struct PerioWithDisabled;
struct Patient;
class PerioStatistic;
class IPatientTileInfo;

struct ToothInfoStr {
	std::string label;
	std::string info;
	std::string notes;
};

struct PriceInfoStr {

	double price;
	double nhif;
};

struct SummaryState
{
	int frameCount{ 0 };
	int currentIdx{ -1 };
	bool buccalSelected{true};
	bool showPerioGraph{false};
	int selectedTooth{ -1 };
	bool noData() const { return frameCount==0; };
};

class IPatientSummaryView
{
public:
	virtual void setPresenter(PatientSummaryPresenter* presenter) = 0;
	virtual IPatientTileInfo* patientTile() = 0;
	virtual void setTeeth(const std::array<ToothPaintHint, 32>& teeth) = 0;
	virtual void setProcedures(const std::vector<Procedure>& p) = 0;
	virtual void setPerioData(const PerioWithDisabled& perio) = 0;
	virtual void setInitialAmbList() = 0;
	virtual void setPerioStatistic(const PerioStatistic& stat) = 0;
	virtual void setDocumentLabel(const std::string& label, const std::string& date, const std::string& doctor) = 0;
	virtual void setToothInfo(const ToothInfoStr& info) = 0;
	virtual void hideToothInfo() = 0;
//	virtual void setPrice(const PriceInfoStr& priceLabel) = 0;
	virtual void setUiState(const SummaryState& state) = 0;

};