#include "ListSelectorPresenter.h"
#include "View/AmbListSelector/IListSelectorView.h"
#include "View/ModalDialogBuilder.h"


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


	view->setDates(_from, _to);
	view->setRows(rows_);
	
}

void ListSelectorPresenter::setDates(const Date& from, const Date& to)
{
	_from = from;
	_to = to;
	refreshModel();
}



void ListSelectorPresenter::refreshModel()
{
	rows_ = amb_db.getAmbListRows(_from, _to);
	
	if (view != nullptr)
	{
		view->setRows(rows_);
		view->setDates(_from, _to);
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
	if (!selectedIndexes.size()) return;

	for(auto idx : selectedIndexes)
	tab_presenter->openList(rows_[idx]);

	if (view) view->close();
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

