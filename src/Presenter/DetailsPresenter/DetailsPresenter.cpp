#include "DetailsPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "View/DetailsView/IDetailsView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Presenter/ListPresenter/StatusPresenter/ToothHintCreator.h"
#include "StatusController.h"
#include "Database/DbProcedure.h"
#include <QDebug>

DetailsPresenter::DetailsPresenter(const Tooth& tooth, const std::string& patientID) :
	tooth(tooth), m_checkModel(tooth), m_detailedStatus(this->tooth), patientID(patientID)
{


}


void DetailsPresenter::okPressed()
{
	m_detailedStatus.stateChanged();
	_result = tooth;
}

void DetailsPresenter::setView(IDetailsView* view) 
{ 
	this->view = view; 
	DbProcedure db;
	view->setHistoryData(db.getToothProcedures(patientID, tooth.index));

	m_detailedStatus.setView(view->detailedStatus());

}



std::optional<Tooth> DetailsPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return _result;
}




DetailsPresenter::~DetailsPresenter()
{

}