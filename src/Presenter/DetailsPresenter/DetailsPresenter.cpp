#include "DetailsPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "View/DetailsView/IDetailsView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"

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

void DetailsPresenter::statusSelected(int category, int code)
{
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

	switch (m_category)
	{
	case StatusType::general:
	{
		switch (m_code)
		{
		case StatusCode::Pulpitis: 
			view->detailedStatus()->setData(tooth.pulpitis.data); break;
		case StatusCode::ApicalLesion:
			view->detailedStatus()->setData(tooth.lesion.data); break;
		case StatusCode::Fracture: 
			view->detailedStatus()->setData(tooth.fracture.data); break;
		case StatusCode::Crown: 
			view->detailedStatus()->setData(tooth.crown.data);
			view->detailedStatus()->setData(tooth.crown.getDentistData()); break;
		case StatusCode::Root: 
			view->detailedStatus()->setData(tooth.root.data); break;
		case StatusCode::EndoTreatment: 
			view->detailedStatus()->setData(tooth.endo.getDentistData()); break;
		case StatusCode::Extraction: 
			view->detailedStatus()->setData(tooth.extraction.getDentistData()); break;
		case StatusCode::Post: 
			view->detailedStatus()->setData(tooth.post.getDentistData()); break;
		case StatusCode::Bridge: 
			view->detailedStatus()->setData(tooth.bridge.data);
			view->detailedStatus()->setData(tooth.bridge.getDentistData()); break;
		case StatusCode::Implant: 
			view->detailedStatus()->setData(tooth.implant.data);
			view->detailedStatus()->setData(tooth.implant.getDentistData()); break;
		}
		break;
	}
	case StatusType::obturation:
	{
		view->detailedStatus()->setData(tooth.obturation[m_code].data);
		view->detailedStatus()->setData(tooth.obturation[m_code].getDentistData()); 
		break;
	}
	case StatusType::caries:
		view->detailedStatus()->setData(tooth.caries[m_code].data); return;
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
