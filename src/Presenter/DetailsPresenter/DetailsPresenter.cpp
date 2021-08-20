#include "DetailsPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "View/DetailsView/IDetailsView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"
#include "StatusController.h"
#include <QDebug>

DetailsPresenter::DetailsPresenter(const Tooth& tooth) :
	tooth(tooth), m_checkModel(tooth)
{
	
}

void DetailsPresenter::checkStateChanged(bool checked)
{
	tooth.setStatus(m_category, m_code, checked);
	m_checkModel = CheckModel(tooth);

	view->detailedStatus()->setCheckModel(m_checkModel);
	view->detailedStatus()->paintTooth(ToothHintCreator::getToothHint(tooth));
	setDynamicDisable();

}

void DetailsPresenter::okPressed()
{
	_result = tooth;
}

void DetailsPresenter::setView(IDetailsView* view) 
{ 
	this->view = view; 

	if (!tooth.bridge.exists())
		view->detailedStatus()->disableItem(StatusCode::Bridge, true);
	if (tooth.type == ToothType::Molar)
		view->detailedStatus()->disableItem(StatusCode::Temporary, true);

	view->detailedStatus()->setCheckModel(m_checkModel);
	view->detailedStatus()->paintTooth(ToothHintCreator::getToothHint(tooth));

}



std::optional<Tooth> DetailsPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return _result;
}

void DetailsPresenter::stateChanged()
{
	if (controller == nullptr) return;
	controller->commitChange();
}

void DetailsPresenter::statusSelected(int category, int code)
{
	controller->commitChange();

	switch (category)
	{
	case -1: m_category = StatusType::general; break;
	case 1: m_category = StatusType::obturation; break;
	case 2: m_category = StatusType::caries; break;
	}

	m_code = code;


	setDynamicStatus();
	setDynamicDisable();
}

void DetailsPresenter::setDynamicStatus()
{
	view->detailedStatus()->clearData();
	
	if (controller != nullptr)
	{
		delete controller;
		controller = nullptr;
	}

	switch (m_category)
	{
		case StatusType::general:
		{
			switch (m_code)
			{
			case StatusCode::Pulpitis:
			{
				PathologyControl* c = new PathologyControl();
				c->setStatus(&tooth.pulpitis);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::ApicalLesion:
			{
				PathologyControl* c = new PathologyControl();
				c->setStatus(&tooth.lesion);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Fracture:
			{
				PathologyControl* c = new PathologyControl();
				c->setStatus(&tooth.fracture);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Crown:
			{
				CrownControl* c = new CrownControl();
				c->setStatus(&tooth.crown);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Root:
			{
				PathologyControl* c = new PathologyControl();
				c->setStatus(&tooth.root);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::EndoTreatment:
			{
				DentistMadeControl* c = new DentistMadeControl();
				c->setStatus(&tooth.endo);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Extraction:
			{
				DentistMadeControl* c = new DentistMadeControl();
				c->setStatus(&tooth.extraction);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Post:
			{
				DentistMadeControl* c = new DentistMadeControl();
				c->setStatus(&tooth.post);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Bridge:
			{
				CrownControl* c = new CrownControl();
				c->setStatus(&tooth.bridge);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			case StatusCode::Implant:
			{
				ImplantControl* c = new ImplantControl();
				c->setStatus(&tooth.implant);
				c->setView(view->detailedStatus());
				controller = c;
			}
			break;

			default: break;
			}
		case StatusType::obturation:
		{
			ObturationControl* c = new ObturationControl();
			c->setStatus(&tooth.obturation[m_code]);
			c->setView(view->detailedStatus());
			controller = c;

		}
		break;
		case StatusType::caries:
		{
			PathologyControl* c = new PathologyControl();
			c->setStatus(&tooth.caries[m_code]);
			c->setView(view->detailedStatus());
			controller = c;
		} break;

		}
	}
}
	

void DetailsPresenter::setDynamicDisable()
{
	switch (m_category)
	{
	case StatusType::general:
		view->detailedStatus()->disableDetails(m_checkModel.generalStatus[m_code] == CheckState::unchecked);
		break;
	case StatusType::obturation:
		view->detailedStatus()->disableDetails(m_checkModel.obturationStatus[m_code] == CheckState::unchecked);
		break;
	case StatusType::caries:
		view->detailedStatus()->disableDetails(m_checkModel.cariesStatus[m_code] == CheckState::unchecked);
		break;
	}
}
