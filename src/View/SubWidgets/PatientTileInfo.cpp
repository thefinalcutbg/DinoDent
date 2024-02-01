#include "PatientTileInfo.h"
#include "Presenter/PatientInfoPresenter.h"

PatientTileInfo::PatientTileInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    connect(ui.patientTile->nraButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->nraClicked(true);
	});

    connect(ui.patientTile, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->patientTileClicked();
	});

    connect(ui.medStatTile, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->medStatTileClicked();
	});

    connect(ui.medStatTile->nhifButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->diagnosisClicked();
	});

    connect(ui.medStatTile->hisButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->allergiesClicked();
	});

    connect (ui.patientTile->printButton, &QPushButton::clicked, this, [=, this] {
		if (presenter) presenter->printDeclarations();
	});
}

void PatientTileInfo::setPatient(const Patient& p, int age)
{
	ui.patientTile->setData(p, age);
	ui.medStatTile->setData(p);
}

PatientTileInfo::~PatientTileInfo()
{}
