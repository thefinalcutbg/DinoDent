#pragma once

#include <unordered_map>
#include "View/TabView/ITabView.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"

#include "../ListPresenter/ListPresenter.h"

class AmbList;
class Patient;
struct AmbRow;
struct PerioRow;

class TabPresenter
{

	std::unordered_map<int, TabInstance*> m_tabs;

	int _indexCounter;
	int m_currentIndex;

	DbAmbList amb_db;
	DbPatient patient_db;

	ITabView* view;

	bool newListExists(const Patient& patient);
	bool ambTabAlreadyOpened(const std::string& ambList_id);
	bool perioTabAlreadyOpened(const std::string& perio_id);

	void openTab(TabInstance* tabInstance);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

public:
	
	TabPresenter();

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);
	void openList(const Patient& patient);
	void openPerio(const Patient& patient);
	void open(const PerioRow& perio);
	void openList(const AmbRow& ambList);
	void removeList(const std::string& ambID);
	void removeCurrentTab();

};

