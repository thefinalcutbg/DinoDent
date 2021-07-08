#include "ListSelectorPresenter.h"
#include "View/AmbListSelector/IListSelectorView.h"
#include "View/ModalDialogBuilder.h"

#include <QDebug>

ListSelectorPresenter::ListSelectorPresenter()
{

}



void ListSelectorPresenter::openDialog()
{
	if (!view) ModalDialogBuilder::openDialog(this);
	else view->focus();
}

void ListSelectorPresenter::setView(IListSelectorView* view)
{
	this->view = view;

	if (!view) return;


	view->addYearsToCombo(amb_db.getValidYears());

	view->setUI(_month, _year);

	view->setRows(rows_);
	
}

void ListSelectorPresenter::setDate(int month, int year)
{

	_month = month; 
	_year = year;

	rows_ = amb_db.getAmbListRows(_month, _year);

	view->setRows(rows_);
}

void ListSelectorPresenter::refreshModel()
{
	rows_ = amb_db.getAmbListRows(_month, _year);
	
	if (view != nullptr)
	{
		view->setRows(rows_);
		view->addYearsToCombo(amb_db.getValidYears());
		view->setUI(_month, _year);
	}
		
}

void ListSelectorPresenter::selectionChanged
(std::vector<int> selectedIndexes) { this->selectedIndexes = std::move(selectedIndexes); }


#include "../TabPresenter/TabPresenter.h"

void ListSelectorPresenter::setTabPresenter(TabPresenter* tabPresenter)
{
	this->tab_presenter = tabPresenter;
}
void ListSelectorPresenter::openAmbList()
{
	for(auto idx : selectedIndexes)
	tab_presenter->openList(rows_[idx]);
}

void ListSelectorPresenter::deleteAmbList()
{
	for (auto idx : selectedIndexes)
	{
		amb_db.deleteAmbList(rows_[idx].id);
		tab_presenter->removeList(rows_[idx].id);
	}

	if(selectedIndexes.size())	
		refreshModel();
	
}

ListSelectorPresenter::~ListSelectorPresenter()
{
}

