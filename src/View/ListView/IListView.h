#pragma once

#include <vector>

#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Model/CheckState.h"


class IListView : public CheckModelReciever
{
public:
	virtual void refresh(AmbList& ambList, Patient &patient, std::vector<int>& selectedIndexes) = 0;
	virtual void repaintTooth(const Tooth& tooth) = 0;
	virtual void repaintBridges(const std::array<BridgeAppearance, 32> bridges) = 0;
	virtual void updateControlPanel(const Tooth* tooth) = 0;
};