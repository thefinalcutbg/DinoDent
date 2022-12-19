#include "PatientTileInfo.h"
#include "Presenter/PatientInfoPresenter.h"

PatientTileInfo::PatientTileInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.patientTile->nraButton, &QPushButton::clicked, [=] {
		if (presenter) presenter->nraClicked(true);
	});

	connect(ui.patientTile, &QPushButton::clicked, [=] {
		if (presenter) presenter->patientTileClicked();
	});

	connect(ui.allergiesTile, &QPushButton::clicked, [=] {
		if (presenter) presenter->allergiesTileClicked();
	});

	connect(ui.allergiesTile->nhifButton, &QPushButton::clicked, [=] {
		if (presenter) presenter->diagnosisClicked();
	});

	connect (ui.patientTile->printButton, &QPushButton::clicked, [=] {
		if (presenter) presenter->printDeclarations();
	});
}

#include <QDebug>

void PatientTileInfo::setPatient(const Patient& p, int age)
{
	ui.patientTile->setData(p, age);
	ui.allergiesTile->setData(p);
}

PatientTileInfo::~PatientTileInfo()
{}
