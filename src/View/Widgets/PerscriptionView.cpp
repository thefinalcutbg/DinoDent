#include "PerscriptionView.h"
#include "Presenter/PerscriptionPresenter.h"

PerscriptionView::PerscriptionView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.addButton->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));
	ui.editButton->setIcon(QIcon(":/icons/icon_edit.png"));

	connect(ui.addButton, &QPushButton::pressed, [=] {if (presenter)presenter->addPressed();});
}

void PerscriptionView::setPatient(const Patient& patient, const Date& currentDate)
{
	ui.patientTile->setData(patient, currentDate);
	ui.allergiesTile->setData(patient);
}

PerscriptionView::~PerscriptionView()
{
}
