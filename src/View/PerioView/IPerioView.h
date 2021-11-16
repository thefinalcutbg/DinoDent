#pragma once

class PerioPresenter;
class ToothPaintHint;
class PerioStatus;
class PerioToothData;
class PerioStatistic;
class Patient;


class IPerioView
{

public:
	virtual void setPatient(const Patient& patient, Date date) = 0;
	virtual void setPresenter(PerioPresenter* presenter) = 0;
	virtual void setToothHint(const ToothPaintHint& hint) = 0;
	virtual void setPerioStatistic(const PerioStatistic& stat) = 0;
	virtual void setMeasurment(int index, int pd, int cal, int gm, int recession) = 0;
	virtual void setAdditional(int smoker, int boneLoss, bool systemic, bool restore) = 0;

	virtual void disableTooth(int index) = 0;
    virtual void setToothData(const PerioToothData& data) = 0;
};