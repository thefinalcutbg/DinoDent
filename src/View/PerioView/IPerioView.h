#pragma once

class PerioPresenter;
class ToothPaintHint;
class PerioStatus;

class IPerioView
{

public:
	virtual void setPresenter(PerioPresenter* presenter) = 0;
	virtual void setToothHint(const ToothPaintHint& hint) = 0;
	virtual void setPerioStatus(const PerioStatus& statu) = 0;
	virtual PerioStatus getPerioStatus() = 0;

};