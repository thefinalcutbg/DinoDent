#pragma once

#include <unordered_map>
#include "View/AmbListPage/TabView/ITabView.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"

#include "../ListPresenter/ListPresenter.h"

class AmbList;
class Patient;
class AmbListRow;

class TabPresenter
{

	std::unordered_map<int, TabInstance*> m_tabs;

	int _indexCounter;
	int m_currentIndex;

	DbAmbList amb_db;
	DbPatient patient_db;

	ITabView* view;

	bool newListExists(const Patient& patient);
	bool listsExists(const std::string& ambList_id);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

public:
	
	TabPresenter();

	void setView(ITabView* view);

	TabInstance* currentTab();
	void setCurrentTab(int index);
	void openList(const Patient& patient);
	void openList(const AmbListRow& ambList);
	void removeList(const std::string& ambID);
	void removeCurrentList();

};

