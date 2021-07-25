#include "DetailsPresenter.h"
#include "View/ModalDialogBuilder.h"

DetailsPresenter::DetailsPresenter()
{

}

void DetailsPresenter::setView(IDetailsView* view) 
{ 
	this->view = view; 


}

std::optional<Tooth> DetailsPresenter::open(const Tooth& tooth)
{
	ModalDialogBuilder::openDialog(this);

	return _tooth;
}

