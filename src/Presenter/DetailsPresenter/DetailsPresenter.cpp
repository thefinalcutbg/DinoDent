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

void DetailsPresenter::statusSelected(int category, int code)
{
	switch (category)
	{
		case -1: m_category = StatusType::general; break;
		case 1: m_category = StatusType::obturation; break;
		case 2: m_category = StatusType::caries; break;
	}

	m_code = code;
}

void DetailsPresenter::checkStateChanged(bool checked)
{
	tooth.setStatus(m_category, m_code, checked);
	m_checkModel = CheckModel(tooth);

	view->detailedStatus()->setCheckModel(m_checkModel);
	view->detailedStatus()->paintTooth(ToothHintCreator::getToothHint(tooth));
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

