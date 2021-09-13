#pragma once
#include <vector>

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



};