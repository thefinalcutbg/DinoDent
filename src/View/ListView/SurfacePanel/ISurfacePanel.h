#pragma once
#include <array>
#include <string>
#include "Model/Tooth/Tooth.h"

class ISurfacePanel {

public:
	virtual void paintTooth(const Tooth* tooth) = 0;
	virtual void showPanel(bool show) = 0;
	virtual void setLabels(std::array<std::string, 6> SurfaceNames) = 0;
	virtual void setSideButtonsClicked(bool obturation, bool caries) = 0;
	virtual void setStatuses(std::array<std::string, 6> StatusNames) = 0;
};