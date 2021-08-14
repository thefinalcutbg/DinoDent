#include "DetailsView.h"
#include "Presenter/DetailsPresenter/DetailsPresenter.h"
#include <QDebug>


DetailsView::DetailsView(DetailsPresenter* p)
	: presenter(p)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Tooth Details");

	p->setView(this);

	connect(ui.status, &DetailedStatus::selectionChanged, [&](int category, int code)
	{
			qDebug() << category << code;
	});

	connect(ui.status, &DetailedStatus::itemChecked, [&]
	{
		qDebug() << "checked";
	});
}

void DetailsView::setCheckModel(const CheckModel& model)
{
	ui.status->setCheckModel(model);
}


DetailsView::~DetailsView()
{

}
