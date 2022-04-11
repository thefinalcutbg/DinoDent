#include "ControlPanel.h"

#include "Presenter/ListPresenter/ListPresenter.h"

ControlPanel::ControlPanel(QWidget *parent)
	: QWidget(parent), presenter(nullptr)
{
	ui.setupUi(this);
	
	statusButtons[StatusCode::Temporary] = ui.Temporary;
	statusButtons[StatusCode::Obturation] = ui.Obturation;
	statusButtons[StatusCode::Caries] = ui.Caries;
	statusButtons[StatusCode::Pulpitis] = ui.Pulpitis;
	statusButtons[StatusCode::ApicalLesion] = ui.ApicalLesion;
	statusButtons[StatusCode::EndoTreatment] = ui.EndoTreatment;
	statusButtons[StatusCode::Fracture] = ui.Fracture;
	statusButtons[StatusCode::Extraction] = ui.Extraction;
	statusButtons[StatusCode::Post] = ui.post;
	statusButtons[StatusCode::Root] = ui.Root;
	statusButtons[StatusCode::Implant] = ui.Implant;
	statusButtons[StatusCode::Periodontitis] = ui.Periodontitis;
	statusButtons[StatusCode::Mobility1] = ui.Mobility1;
	statusButtons[StatusCode::Mobility2] = ui.Mobility2;
	statusButtons[StatusCode::Mobility3] = ui.Mobility3;
	statusButtons[StatusCode::Crown] = ui.Crown;
	statusButtons[StatusCode::Bridge] = ui.Bridge;
	statusButtons[StatusCode::FiberSplint] = ui.Fiber;
	statusButtons[StatusCode::Dsn] = ui.Dsn;
	statusButtons[StatusCode::Impacted] = ui.Impacted;
	
	StatusButton* pathologies[12]
	{
		ui.Caries, ui.Pulpitis, ui.Extraction, ui.ApicalLesion, ui.Fracture, ui.Periodontitis, ui.Mobility1,
		ui.Mobility2, ui.Mobility3, ui.Dsn, ui.Impacted, ui.Root
	};

	for (auto& p : pathologies)
	{
		p->pathology = true;
	}


	for (int i = 0; i<statusButtons.size(); i++)
	{
		connect( statusButtons[i], &QPushButton::pressed, this, [=] {

			if (presenter == NULL) return;
			presenter->setMainStatus(i); 
			
			} );
	}

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
	for (int i = 0; i < statusButtons.size(); i++)
	{
		statusButtons[i]->setCheckState(checkModel.generalStatus[i]);
	}
}
