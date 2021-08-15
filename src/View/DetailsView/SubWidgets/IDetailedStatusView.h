#pragma once

class CheckModel;
class ToothPaintHint;

class IDetailedStatusView
{
public:
	virtual void setCheckModel(const CheckModel& model) = 0;
	virtual void disableItem(int index, bool disabled) = 0;
	virtual void paintTooth(const ToothPaintHint& hint) = 0;
};