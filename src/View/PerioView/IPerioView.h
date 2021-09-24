#pragma once

class PerioPresenter;
class ToothPaintHint;

#include "Model/PerioStatus.h"
#include <tuple>

class IPerioView
{

public:
	virtual void setPresenter(PerioPresenter* presenter) = 0;
	virtual void setToothHint(const ToothPaintHint& hint) = 0;
	virtual void setPerioStatus(const PerioStatus& upper, const PerioStatus& lower) = 0;
	virtual std::tuple<PerioStatus, PerioStatus> getPerioStatus() = 0;

};