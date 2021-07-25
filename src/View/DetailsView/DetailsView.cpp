#include "DetailsView.h"
#include "Presenter/DetailsPresenter/DetailsPresenter.h"

DetailsView::DetailsView(DetailsPresenter* p)
	: presenter(p)
{
	ui.setupUi(this);
	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Tooth Details");


	p->setView(this);

}

DetailsView::~DetailsView()
{
}
