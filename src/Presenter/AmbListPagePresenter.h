#pragma once

#include <vector>

#include "View/AmbListPage/IAmbListPage.h"
#include "TabPresenter/ListInstance.h"
#include "Database/DbAmbList.h"
#include "TabPresenter/TabPresenter.h"
#include "ListSelector/ListSelectorPresenter.h"

class AmbListPagePresenter
{
	TabPresenter tabPresenter_;
	ListSelectorPresenter listSelector_;
	IAmbListPage* view;

	DbAmbList amb_db;

public:
	AmbListPagePresenter();

	void setView(IAmbListPage* view);
	void newPressed();
	void showListSelector();
	bool save();
	bool saveAs();

	//returns false when user selects cancel, otherwise returns true
	bool closeTab();
	//returns false when user selects cancel, otherwise returns true
	bool closeAllTabs();
};

