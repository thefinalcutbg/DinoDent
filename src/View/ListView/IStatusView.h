#pragma once

class ToothPaintHint;
class BridgesPaintHint;
class Tooth;
class StatusPresenter;
class CheckModel;

class IStatusView
{

public:

	virtual void setStatusControlPresenter(StatusPresenter* presenter) = 0;

	virtual void setCheckModel(const CheckModel& checkModel) = 0;
	virtual void setSelectedTeeth(std::vector<int> selectedTeeth) = 0;

	virtual void repaintTooth(const ToothPaintHint& tooth) = 0;
	virtual void repaintBridges(const BridgesPaintHint& bridges) = 0;
	virtual void updateControlPanel(const Tooth* tooth) = 0;

};