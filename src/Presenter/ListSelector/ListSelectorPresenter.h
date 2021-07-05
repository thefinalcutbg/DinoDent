#pragma once

#include "Model/AmbListRow.h"
#include "Database/DbAmbList.h"

class IListSelectorView;

class ListSelectorPresenter
{
	IListSelectorView* view{ nullptr };
	DbAmbList _db;

	int
	_index{ -1 },
	_month{ Date::currentMonth() },
	_year{ Date::currentYear() };
	
	std::vector<AmbListRow> _rows{ _db.getAmbListRows(_month, _year) };



public:
	ListSelectorPresenter();
	
	void openDialog();

	void setView(IListSelectorView* view);

	void setDate(int month, int year);

	void refreshModel();

};

