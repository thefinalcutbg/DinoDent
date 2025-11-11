#include "ControlPanel.h"

#include "Presenter/ListPresenter.h"
#include <QMenu>
#include "DsnMenu.h"

ControlPanel::ControlPanel(QWidget* parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
#ifdef DISABLE_NEW_DENTAL_STATUSES
	ui.verticalLayout->removeWidget(ui.Temporary);
	ui.Necrosis->hide();
	ui.NonCaries->hide();
	ui.DefResto->hide();
	ui.Resorption->hide();
	ui.verticalLayout_2->addWidget(ui.Temporary);
#endif
	StatusButton* pathologies[]
	{
		ui.Caries, ui.NonCaries, ui.Resorption, ui.Necrosis, ui.Pulpitis, ui.Missing, ui.ApicalLesion, ui.Fracture, ui.Periodontitis, ui.Dsn, ui.Impacted, ui.Root, ui.Calculus
	};

	for (auto& p : pathologies) p->pathology = true;

	ui.Mobility->pathology = true;


	auto lambdaConnect = [this](QPushButton* button, Dental::Status code)
	{
        this->connect(button, &QPushButton::clicked, this, [=, this] {

            if (presenter == nullptr) return;
				presenter->setToothStatus(Dental::StatusType::General, code);
			});
	};

	lambdaConnect(ui.healthyTooth, Dental::Healthy);
	lambdaConnect(ui.ApicalLesion, Dental::ApicalLesion);
	lambdaConnect(ui.Bridge, Dental::Bridge);
	lambdaConnect(ui.Caries, Dental::Caries);
	lambdaConnect(ui.NonCaries, Dental::NonCariesLesion);
	lambdaConnect(ui.DefResto, Dental::DefectiveRestoration);
	lambdaConnect(ui.Crown, Dental::Crown);
	lambdaConnect(ui.Dsn, Dental::HasSupernumeral);
	lambdaConnect(ui.RootCanal, Dental::RootCanal);
	lambdaConnect(ui.Missing, Dental::Missing);
	lambdaConnect(ui.Fiber, Dental::Splint);
	lambdaConnect(ui.Fracture, Dental::Fracture);
	lambdaConnect(ui.Implant, Dental::Implant);
	lambdaConnect(ui.Calculus, Dental::Calculus);
	lambdaConnect(ui.Impacted, Dental::Impacted);
	lambdaConnect(ui.Restoration, Dental::Restoration);
	lambdaConnect(ui.Periodontitis, Dental::Periodontitis);
	lambdaConnect(ui.post, Dental::Post);
	lambdaConnect(ui.Pulpitis, Dental::Pulpitis);
	lambdaConnect(ui.Necrosis, Dental::Necrosis);
	lambdaConnect(ui.Resorption, Dental::Resorption);
	lambdaConnect(ui.Root, Dental::Root);
	lambdaConnect(ui.Temporary, Dental::Temporary);
	lambdaConnect(ui.falseTooth, Dental::Denture);

	ui.Mobility->setStateNames({ "Подвижност", "Подвижност I", "Подвижност II", "Подвижност III" });

    connect(ui.Mobility, &StatusMultiButton::stateChanged, this, [this](int state)
		{
			if (!presenter) return;
			
			switch (state)
			{
				case 0:presenter->setToothStatus(Dental::StatusType::General, Dental::Mobility); break;
				case 1:presenter->setToothStatus(Dental::StatusType::Mobility, 0); break;
				case 2:presenter->setToothStatus(Dental::StatusType::Mobility, 1); break;
				case 3:presenter->setToothStatus(Dental::StatusType::Mobility, 2); break;
			}
		}
	);

    connect(ui.unknown, &QPushButton::clicked, this, [=, this] {
		if (presenter)
			presenter->setOther(OtherInputs::removeAll);
	});
	
	menu = new DsnMenu();
	ui.Dsn->setMenu(menu);

}

ControlPanel::~ControlPanel()
{
	delete menu;
}

void ControlPanel::setPresenter(ListPresenter* presenter)
{
	this->presenter = presenter;
	menu->setPresenter(presenter);
}

void ControlPanel::hideCommonButtons(bool hidden)
{
	ui.Restoration->setHidden(hidden);
	ui.Caries->setHidden(hidden);
#ifndef DISABLE_NEW_DENTAL_STATUSES
	ui.NonCaries->setHidden(hidden);
	ui.DefResto->setHidden(hidden);
#endif
}

void ControlPanel::setModel(const CheckModel& checkModel, const CheckModel& dsn)
{

	auto setCheck = [&](StatusButton* b, Dental::Status s) {
		b->setCheckState(checkModel.generalStatus[s]);
	};

	setCheck(ui.healthyTooth, Dental::Healthy);
	setCheck(ui.ApicalLesion, Dental::ApicalLesion);
	setCheck(ui.Bridge, Dental::Bridge);
	setCheck(ui.Caries, Dental::Caries);
	setCheck(ui.Crown, Dental::Crown);
	setCheck(ui.Dsn, Dental::HasSupernumeral);
	setCheck(ui.RootCanal, Dental::RootCanal);
	setCheck(ui.Missing, Dental::Missing);
	setCheck(ui.Fiber, Dental::Splint);
	setCheck(ui.Fracture, Dental::Fracture);
	setCheck(ui.Impacted, Dental::Impacted);
	setCheck(ui.Implant, Dental::Implant);
	setCheck(ui.Restoration, Dental::Restoration);
	setCheck(ui.Periodontitis, Dental::Periodontitis);
	setCheck(ui.post, Dental::Post);
	setCheck(ui.Pulpitis, Dental::Pulpitis);
	setCheck(ui.Root, Dental::Root);
	setCheck(ui.Temporary, Dental::Temporary);
	setCheck(ui.falseTooth, Dental::Denture);
	setCheck(ui.Calculus, Dental::Calculus);
	setCheck(ui.DefResto, Dental::DefectiveRestoration);
	setCheck(ui.NonCaries, Dental::NonCariesLesion);
	setCheck(ui.Resorption, Dental::Resorption);
	setCheck(ui.Necrosis, Dental::Necrosis);

	ui.unknown->setCheckState(checkModel.no_data ? CheckState::checked : CheckState::unchecked);

	int mobilityState = 0;

	for (int i = 0; i < Dental::MobilityCount; i++)
	{
		if (checkModel.mobilityStatus[i] == CheckState::checked) {
			mobilityState = i + 1;
			break;
		}
	}

	ui.Mobility->setCurrentState(mobilityState);
	
	menu->setModel(dsn);
}
