#include "DetailedStatusPresenter.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"
#include "../StatusController.h"

DetailedStatusPresenter::DetailedStatusPresenter(Tooth& tooth) :
	tooth(tooth), m_checkModel(tooth)
{
}

void DetailedStatusPresenter::setView(IDetailedStatusView* view)
{
	this->view = view; 
	view->setPresenter(this);

	if (!tooth.bridge.exists())
		view->disableItem(StatusCode::Bridge, true);
	if (tooth.type == ToothType::Molar)
		view->disableItem(StatusCode::Temporary, true);

	view->setCheckModel(m_checkModel);
	view->paintTooth(ToothHintCreator::getToothHint(tooth));
}

void DetailedStatusPresenter::checkStateChanged(bool checked)
{
	tooth.setStatus(m_category, m_code, checked);
	m_checkModel = CheckModel(tooth);

	view->setCheckModel(m_checkModel);
	view->paintTooth(ToothHintCreator::getToothHint(tooth));

	setDynamicDisable();
}

void DetailedStatusPresenter::stateChanged()
{
	if (controller) controller->applyChange();
	view->paintTooth(ToothHintCreator::getToothHint(tooth));
}

void DetailedStatusPresenter::statusSelected(int category, int code)
{
	if (controller) controller->applyChange();

	m_code = code;

	view->clearData();

	switch (category)
	{
	case -1:
	{
		m_category = StatusType::general;

		switch (m_code)
		{
			case StatusCode::Pulpitis:
				controller = std::make_unique<PathologyControl>(*view, tooth.pulpitis); break;
			case StatusCode::ApicalLesion:
				controller = std::make_unique <PathologyControl>(*view, tooth.lesion); break;
			case StatusCode::Fracture:
				controller = std::make_unique <PathologyControl>(*view, tooth.fracture); break;
			case StatusCode::Crown:
				controller = std::make_unique<CrownControl>(*view, tooth.crown); break;
			case StatusCode::Root:
				controller = std::make_unique <PathologyControl>(*view, tooth.root); break;
			case StatusCode::EndoTreatment:
				controller = std::make_unique<DentistMadeControl>(*view, tooth.endo); break;
			case StatusCode::Extraction:
				controller = std::make_unique<DentistMadeControl>(*view, tooth.extraction); break;
			case StatusCode::Post:
				controller = std::make_unique<DentistMadeControl>(*view, tooth.post); break;
			case StatusCode::Bridge:
				controller = std::make_unique<CrownControl>(*view, tooth.bridge); break;
			case StatusCode::Implant:
				controller = std::make_unique<ImplantControl>(*view, tooth.implant); break;
			default: break;
		}

		break;
	}
	case 1:
		m_category = StatusType::obturation;
		controller = std::make_unique<ObturationControl>(*view, tooth.obturation[m_code]); break;
	case 2:
		m_category = StatusType::caries;
		controller = std::make_unique <PathologyControl>(*view, tooth.caries[m_code]); break;
	}

	setDynamicDisable();

}

void DetailedStatusPresenter::setDynamicDisable()
{
	switch (m_category)
	{
	case StatusType::general:
		view->disableDetails(m_checkModel.generalStatus[m_code] == CheckState::unchecked);
		break;
	case StatusType::obturation:
		view->disableDetails(m_checkModel.obturationStatus[m_code] == CheckState::unchecked);
		break;
	case StatusType::caries:
		view->disableDetails(m_checkModel.cariesStatus[m_code] == CheckState::unchecked);
		break;
	}
	
}
