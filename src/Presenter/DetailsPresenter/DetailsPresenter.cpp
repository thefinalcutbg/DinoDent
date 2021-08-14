#include "DetailsPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "View/DetailsView/IDetailsView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include <QDebug>
DetailsPresenter::DetailsPresenter(const Tooth& tooth) :
	tooth(tooth)
{

}

void DetailsPresenter::setView(IDetailsView* view) 
{ 
	this->view = view; 

	view->setCheckModel(CheckModel{ std::vector<Tooth*>{&this->tooth} });

}

std::optional<Tooth> DetailsPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return _result;
}

