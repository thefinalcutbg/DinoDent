#pragma once
#include <vector>
#include "ListInstance.h"
#include "View/AmbListPage/TabView/ITabView.h"
#include "Database/DbAmbList.h"

#include "../ListPresenter/ListPresenter.h"

class AmbList;
class Patient;

class TabPresenter : public EditObserver
{
	std::vector<ListInstance> _lists;

	int _index;

	DbAmbList database;

	ITabView* view;
	ListPresenter _listPresenter;
	bool listExists(const Patient& patient);
	bool listsExist(const std::string& ambList_id);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

public:
	
	TabPresenter();

	void editNotify() override;
	void setView(ITabView* view);

	ListInstance* currentList();
	void setCurrentList(int index);
	void newList(const Patient& patient);
	void removeCurrentList();

};

