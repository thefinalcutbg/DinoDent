#pragma once

#include <QDialog>
#include "ui_DetailsView.h"
#include "IDetailsView.h"


class DetailsPresenter;

class DetailsView : public QDialog, public IDetailsView
{
	Q_OBJECT

	DetailsPresenter* presenter;

public:
	DetailsView(DetailsPresenter* p);

	IDetailedStatusView* detailedStatus() override;
	void setHistoryData(const std::vector<DetailsSummary>& history) override;
	~DetailsView();

private:
	Ui::DetailsView ui;
};
