#pragma once
#include <vector>
#include "ListInstance.h"
#include "View/AmbListPage/TabView/ITabView.h"
#include "Database/DbAmbList.h"
#include "Database/DbPatient.h"

#include "../ListPresenter/ListPresenter.h"

class AmbList;
class Patient;
class AmbListRow;

class TabPresenter : public EditObserver
{
	std::vector<ListInstance> lists_;

	int index_;

	DbAmbList amb_db;
	DbPatient patient_db;

	ITabView* view;
	ListPresenter listPresenter_;
	bool listExists(const Patient& patient);
	bool listsExist(const std::string& ambList_id);

	std::shared_ptr<Patient> getPatient_ptr(const Patient& patient);

public:
	
	TabPresenter();

	void editNotify() override;
	void setView(ITabView* view);

	ListInstance* currentList();
	void setCurrentList(int index);
	void openList(const Patient& patient);
	void openList(const AmbListRow& ambList);
	void removeList(const std::string& ambID);
	void removeCurrentList();

};

