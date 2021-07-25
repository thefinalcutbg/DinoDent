#pragma once

class ToothPaintHint;
class BridgesPaintHint;
class Tooth;
class StatusPresenter;
class CheckModel;

class ISurfacePanel;

class IStatusView
{

public:

	virtual void setStatusControlPresenter(StatusPresenter* presenter) = 0;

	virtual void setCheckModel(const CheckModel& checkModel) = 0;
	virtual void setSelectedTeeth(std::vector<int> selectedTeeth) = 0;

	virtual void hideSurfacePanel(bool hidden) = 0;
	virtual void hideControlPanel(bool hidden) = 0;
	virtual ISurfacePanel* surfacePanel() = 0;

	virtual void repaintTooth(const ToothPaintHint& tooth) = 0;
	virtual void repaintBridges(const BridgesPaintHint& bridges) = 0;

};