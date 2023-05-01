#include "ControlPanel.h"

#include "Presenter/ListPresenter.h"



ControlPanel::ControlPanel(QWidget* parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);

	StatusButton* pathologies[]
	{
		ui.Caries, ui.Pulpitis, ui.Extraction, ui.ApicalLesion, ui.Fracture, ui.Periodontitis, ui.Dsn, ui.Impacted, ui.Root, ui.Calculus
	};

	for (auto& p : pathologies) p->pathology = true;

	ui.Mobility->pathology = true;


	auto lambdaConnect = [this](QPushButton* button, StatusCode::StatusCode code)
	{
		this->connect(button, &QPushButton::clicked, this, [=] {

			if (presenter == NULL) return;
				presenter->setMainStatus(code);
			});
	};

	lambdaConnect(ui.healthyTooth, StatusCode::Healthy);
	lambdaConnect(ui.ApicalLesion, StatusCode::ApicalLesion);
	lambdaConnect(ui.Bridge, StatusCode::Bridge);
	lambdaConnect(ui.Caries, StatusCode::Caries);
	lambdaConnect(ui.Crown, StatusCode::Crown);
	lambdaConnect(ui.Dsn, StatusCode::Dsn);
	lambdaConnect(ui.EndoTreatment, StatusCode::EndoTreatment);
	lambdaConnect(ui.Extraction, StatusCode::Extraction);
	lambdaConnect(ui.Fiber, StatusCode::FiberSplint);
	lambdaConnect(ui.Fracture, StatusCode::Fracture);
	lambdaConnect(ui.Implant, StatusCode::Implant);
	lambdaConnect(ui.Calculus, StatusCode::Calculus);
	lambdaConnect(ui.Impacted, StatusCode::Impacted);
	lambdaConnect(ui.Obturation, StatusCode::Obturation);
	lambdaConnect(ui.Periodontitis, StatusCode::Periodontitis);
	lambdaConnect(ui.post, StatusCode::Post);
	lambdaConnect(ui.Pulpitis, StatusCode::Pulpitis);
	lambdaConnect(ui.Root, StatusCode::Root);
	lambdaConnect(ui.Temporary, StatusCode::Temporary);
	lambdaConnect(ui.falseTooth, StatusCode::Denture);
	

	ui.Mobility->setStateNames({ "Подвижност", "Подвижност I", "Подвижност II", "Подвижност III" });

	connect(ui.Mobility, &StatusMultiButton::stateChanged, [this](int state)
		{
			if (!presenter) return;
			
			switch (state)
			{
				case 0:presenter->setMainStatus(StatusCode::Mobility); break;
				case 1:presenter->setMobility(0); break;
				case 2:presenter->setMobility(1); break;
				case 3:presenter->setMobility(2); break;
			}
		}
	);

	connect(ui.unknown, &QPushButton::clicked, this, [=] {
		if (presenter)
			presenter->setOther(OtherInputs::removeAll);
	});
}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
}

void ControlPanel::hideCommonButtons(bool hidden)
{
	ui.Obturation->setHidden(hidden);
	ui.Caries->setHidden(hidden);
}

void ControlPanel::setModel(const CheckModel& checkModel)
{

	auto setCheck = [&](StatusButton* b, StatusCode::StatusCode s) {
		b->setCheckState(checkModel.generalStatus[s]);
	};

	setCheck(ui.healthyTooth, StatusCode::Healthy);
	setCheck(ui.ApicalLesion, StatusCode::ApicalLesion);
	setCheck(ui.Bridge, StatusCode::Bridge);
	setCheck(ui.Caries, StatusCode::Caries);
	setCheck(ui.Crown, StatusCode::Crown);
	setCheck(ui.Dsn, StatusCode::Dsn);
	setCheck(ui.EndoTreatment, StatusCode::EndoTreatment);
	setCheck(ui.Extraction, StatusCode::Extraction);
	setCheck(ui.Fiber, StatusCode::FiberSplint);
	setCheck(ui.Fracture, StatusCode::Fracture);
	setCheck(ui.Impacted, StatusCode::Impacted);
	setCheck(ui.Implant, StatusCode::Implant);
	setCheck(ui.Obturation, StatusCode::Obturation);
	setCheck(ui.Periodontitis, StatusCode::Periodontitis);
	setCheck(ui.post, StatusCode::Post);
	setCheck(ui.Pulpitis, StatusCode::Pulpitis);
	setCheck(ui.Root, StatusCode::Root);
	setCheck(ui.Temporary, StatusCode::Temporary);
	setCheck(ui.falseTooth, StatusCode::Denture);
	setCheck(ui.Calculus, StatusCode::Calculus);
	
	ui.unknown->setCheckState(checkModel.no_data ? CheckState::checked : CheckState::unchecked);

	if (checkModel.mobilityStatus[0] == CheckState::checked) {
		ui.Mobility->setCurrentState(1); return;
	}

	if (checkModel.mobilityStatus[1] == CheckState::checked) {
		ui.Mobility->setCurrentState(2); return;
	}

	if (checkModel.mobilityStatus[2] == CheckState::checked) {
		ui.Mobility->setCurrentState(3); return;
	}

	ui.Mobility->setCurrentState(0);
	
}
