#pragma once

#include <QWidget>
#include "ui_PerscriptionView.h"

#include "View/Interfaces/IPerscriptionView.h"

class PerscriptionView : public QWidget, public IPerscriptionView
{
	Q_OBJECT

	PerscriptionPresenter* presenter{ nullptr };

public:
	PerscriptionView(QWidget *parent = nullptr);
	void setPresenter(PerscriptionPresenter* p) override { presenter = p;}
	void setPatient(const Patient& patient, const Date& currentDate);
	~PerscriptionView();

private:
	Ui::PerscriptionViewClass ui;
};
