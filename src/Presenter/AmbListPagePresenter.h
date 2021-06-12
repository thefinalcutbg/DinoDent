#pragma once

#include <vector>

#include "View/AmbListPage/IAmbListPage.h"
#include "TabPresenter/ListInstance.h"
#include "Database/DbAmbList.h"
#include "TabPresenter/TabPresenter.h"

class AmbListPagePresenter
{
	TabPresenter _tabPresenter;

	IAmbListPage* view;

	ListPresenter* listPresenter;
	std::vector<ListInstance> list_instance;
	DbAmbList database;


	int currentVecPos;


public:
	AmbListPagePresenter( 
						IAmbListPage* AmbListPage, 
						
						ListPresenter* listView);


	void newPressed();



	bool save();
	bool saveAs();
	bool closeTab();
};

