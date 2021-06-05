#include "ControlPanel.h"

#include "Presenter/ListPresenter/StatusPresenter/StatusPresenter.h"

ControlPanel::ControlPanel(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
	
	statusButtons[static_cast<int>(StatusCode::Temporary)] = ui.Temporary;
	statusButtons[static_cast<int>(StatusCode::Obturation)] = ui.Obturation;
	statusButtons[static_cast<int>(StatusCode::Caries)] = ui.Caries;
	statusButtons[static_cast<int>(StatusCode::Pulpitis)] = ui.Pulpitis;
	statusButtons[static_cast<int>(StatusCode::ApicalLesion)] = ui.ApicalLesion;
	statusButtons[static_cast<int>(StatusCode::EndoTreatment)] = ui.EndoTreatment;
	statusButtons[static_cast<int>(StatusCode::Fracture)] = ui.Fracture;
	statusButtons[static_cast<int>(StatusCode::Extraction)] = ui.Extraction;
	statusButtons[static_cast<int>(StatusCode::Post)] = ui.post;
	statusButtons[static_cast<int>(StatusCode::Root)] = ui.Root;
	statusButtons[static_cast<int>(StatusCode::Implant)] = ui.Implant;
	statusButtons[static_cast<int>(StatusCode::Periodontitis)] = ui.Periodontitis;
	statusButtons[static_cast<int>(StatusCode::Mobility1)] = ui.Mobility1;
	statusButtons[static_cast<int>(StatusCode::Mobility2)] = ui.Mobility2;
	statusButtons[static_cast<int>(StatusCode::Mobility3)] = ui.Mobility3;
	statusButtons[static_cast<int>(StatusCode::Crown)] = ui.Crown;
	statusButtons[static_cast<int>(StatusCode::Bridge)] = ui.Bridge;
	statusButtons[static_cast<int>(StatusCode::Dsn)] = ui.Dsn;
	

	for (int i = 0; i<statusButtons.size(); i++)
	{
		connect( statusButtons[i], &QPushButton::clicked, this, [=] {

			if (presenter == NULL) return;
			presenter->changeStatus(static_cast<StatusAction>(i)); 
			
			} );
	}

}

ControlPanel::~ControlPanel()
{
}

void ControlPanel::setPresenter(StatusPresenter* presenter)
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
	for (int i = 0; i < statusButtons.size(); i++)
	{
		statusButtons[i]->setCheckState(checkModel.generalStatus[i]);
	}
}
