#include "ListSelectorPresenter.h"
#include "View/AmbListSelector/IListSelectorView.h"
#include "View/ModalDialogBuilder.h"

#include <QDebug>

ListSelectorPresenter::ListSelectorPresenter()
{

}

void ListSelectorPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);
}

void ListSelectorPresenter::setView(IListSelectorView* view)
{
	this->view = view;

	if (!view) return;

	auto years = _db.getValidYears();

	for (int& year : years)
		view->addYearToCombo(year);

	view->setUI(_month, _year);

	view->setRows(_rows);
	
}

void ListSelectorPresenter::setDate(int month, int year)
{

	_month = month; 
	_year = year;

	_rows = _db.getAmbListRows(_month, _year);

	view->setRows(_rows);
}

void ListSelectorPresenter::refreshModel()
{
	_rows = _db.getAmbListRows(_month, _year);

	if(view != nullptr)
		view->setRows(_rows);
}
