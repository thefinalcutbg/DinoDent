#pragma once
#include <array>

#include "Model/Tooth/Tooth.h";
#include "View/ListView/IListView.h"

std::array<BridgeAppearance, 32> StatusToUIBridge(std::array<Tooth, 32>& teeth) {
	
	std::array<BridgeAppearance, 32> bridgeAppearance;
	
	for (int i = 0; i < teeth.size(); i++) {

		Tooth& tooth = teeth.at(i);

		if (!tooth.bridge.exists())
		{
			bridgeAppearance[i] = BridgeAppearance::None;
			continue;
		}

		if (tooth.bridge.position == BridgePos::Middle) {
			bridgeAppearance[i] = BridgeAppearance::Middle;
		}
		else {
			bridgeAppearance[i] = BridgeAppearance::Terminal;
		}
	}
	return bridgeAppearance;
}

