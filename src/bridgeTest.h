#pragma once
#include "Model/Tooth/Tooth.h"
#include "Presenter/ListPresenter/BridgeController.h"
#include <QDebug>
/*
bool testBridge1() {

	//formats selected cosecutive teeth into one bridge;

	std::array<Tooth, 32>* teeth = new std::array<Tooth, 32>;
	teeth->at(0).bridge.m_exists = true;
	teeth->at(1).bridge.m_exists = true;
	teeth->at(2).bridge.m_exists = true;
	teeth->at(3).bridge.m_exists = true;

	std::vector<int> selectedIdx{0,1,2,3};

	BridgeController controller;

	controller.formatBridges(selectedIdx, teeth);

	if (teeth->at(0).bridge.position != BridgePos::end) return false;
	if (teeth->at(1).bridge.position != BridgePos::middle) return false;
	if (teeth->at(2).bridge.position != BridgePos::middle) return false;
	if (teeth->at(3).bridge.position != BridgePos::end) return false;

	delete teeth;

	return true;
}


bool testBridge2() {

	//formats selected cosecutive teeth into one bridge;

	std::array<Tooth, 32>* teeth = new std::array<Tooth, 32>;

	//making a second bridge:
	teeth->at(1).bridge.m_exists = true;
	teeth->at(1).bridge.position = BridgePos::end;
	teeth->at(2).bridge.m_exists = true;
	teeth->at(2).bridge.position = BridgePos::middle;
	teeth->at(3).bridge.m_exists = true;
	teeth->at(3).bridge.position = BridgePos::middle;
	teeth->at(4).bridge.m_exists = true;
	teeth->at(4).bridge.position = BridgePos::end;

	teeth->at(5).bridge.m_exists = true;
	teeth->at(6).bridge.m_exists = true;

	std::vector<int> selectedIdx{ 3,4,5,6};

	BridgeController controller;

	controller.formatBridges(selectedIdx, teeth);

	if (teeth->at(1).bridge.position != BridgePos::end) return false;
	if (teeth->at(2).bridge.position != BridgePos::end) return false;
	if (teeth->at(3).bridge.position != BridgePos::end) return false;
	if (teeth->at(4).bridge.position != BridgePos::middle) return false;
	if (teeth->at(5).bridge.position != BridgePos::middle) return false;
	if (teeth->at(6).bridge.position != BridgePos::end) return false;

	delete teeth;

	return true;
}

bool testBridge3() {

	//formats selected cosecutive teeth into one bridge;

	std::array<Tooth, 32>* teeth = new std::array<Tooth, 32>;

	//making a second bridge:
	teeth->at(1).bridge.m_exists = true;
	teeth->at(1).bridge.position = BridgePos::end;
	teeth->at(2).bridge.m_exists = true;
	teeth->at(2).bridge.position = BridgePos::middle;
	teeth->at(3).bridge.m_exists = true;
	teeth->at(3).bridge.position = BridgePos::middle;
	teeth->at(4).bridge.m_exists = true;
	teeth->at(4).bridge.position = BridgePos::end;

	teeth->at(3).bridge.m_exists = false;
	teeth->at(4).bridge.m_exists = false;
	teeth->at(5).bridge.m_exists = false;
	teeth->at(6).bridge.m_exists = false;

	std::vector<int> selectedIdx{ 3,4,5,6 };

	BridgeController controller;

	controller.formatBridges(selectedIdx, teeth);

	if (teeth->at(1).bridge.position != BridgePos::end) return false;
	if (teeth->at(2).bridge.position != BridgePos::end) return false;

	delete teeth;

	return true;
}
*/