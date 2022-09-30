#include "PatientTileInfo.h"
#include "Presenter/PatientInfoPresenter.h"

PatientTileInfo::PatientTileInfo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.patientTile->nraIcon, &QPushButton::clicked, [=] {
		if (presenter) presenter->nraClicked(true);
	});

	connect(ui.patientTile, &QPushButton::clicked, [=] {
		if (presenter) presenter->patientTileClicked();
	});

	connect(ui.allergiesTile, &QPushButton::clicked, [=] {
		if (presenter) presenter->allergiesTileClicked();
	});

	connect(ui.allergiesTile->nzokIcon, &QPushButton::clicked, [=] {
		if (presenter) presenter->diagnosisClicked();
	});
}

void PatientTileInfo::setPatient(const Patient& p, int age)
{
	ui.patientTile->setData(p, age);
	ui.allergiesTile->setData(p);
}

PatientTileInfo::~PatientTileInfo()
{}
