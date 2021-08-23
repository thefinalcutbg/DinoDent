#include "DetailsView.h"
#include "Presenter/DetailsPresenter/DetailsPresenter.h"
#include <QDebug>


DetailsView::DetailsView(DetailsPresenter* p)
	: presenter(p)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowFlags(Qt::Window);
	setWindowTitle("Tooth Details");

	connect(ui.okButton, &QPushButton::clicked, [&] {presenter->okPressed(); close(); });
	connect(ui.cancelButton, &QPushButton::clicked, [&] { close(); });

	p->setView(this);
}

IDetailedStatusView* DetailsView::detailedStatus()
{
	return ui.status;
}

void DetailsView::setHistoryData(const std::vector<DetailsSummary>& history)
{
	ui.history->setModel(history);
}

DetailsView::~DetailsView()
{

}
