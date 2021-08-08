#pragma once

#include <array>
#include <vector>

#include "Model/Tooth/Tooth.h"

class BridgeController
{

public:
	BridgeController();
	void formatBridges(const std::vector<int>& indexes, std::array<Tooth, 32>* teeth);
	void removeBridge(int tooth_idx, std::array<Tooth, 32>* teeth);

};

