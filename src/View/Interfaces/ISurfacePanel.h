#pragma once
#include <array>
#include <string>

class ToothPaintHint;
class SurfacePanelPresenter;


class ISurfacePanel {

public:
	
	virtual void setPresenter(SurfacePanelPresenter* presenter) = 0;
	virtual void paintTooth(const ToothPaintHint& tooth) = 0;
	virtual void hidePanel(bool hidden) = 0;
	virtual void setLabels(std::array<std::string, 10> SurfaceNames) = 0;
	virtual void setSideButtonsClicked(bool obturation, bool caries) = 0;
};