#pragma once

class PerioPresenter;
class IPatientTileInfo;

class PerioToothData;
class PerioStatistic;
class Patient;

struct Date;
struct PerioStatus;
struct ToothPaintHint;

enum class ShowTeeth {ShowUpperTeeth, ShowLowerTeeth};

class IPerioView
{

public:
	virtual IPatientTileInfo* patientTile() = 0;
	virtual void setPresenter(PerioPresenter* presenter) = 0;
	virtual void setToothHint(const ToothPaintHint& hint) = 0;
	virtual void setPerioStatistic(const PerioStatistic& stat) = 0;
	virtual void setMeasurment(int index, int pd, int cal, int gm, int recession) = 0;
	virtual void setAdditional(int smoker, int boneLoss, bool systemic, bool restore) = 0;
	virtual void setTeethView(ShowTeeth t) = 0;
	virtual void disableTooth(int index) = 0;
    virtual void setToothData(const PerioToothData& data) = 0;
	virtual void setDate(const Date& date) = 0;
};