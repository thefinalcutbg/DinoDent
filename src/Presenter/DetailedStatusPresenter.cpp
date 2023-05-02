#include "DetailedStatusPresenter.h"
#include "Presenter/CheckState.h"
#include "Presenter/ToothHintCreator.h"
#include "View/ModalDialogBuilder.h"

#include "Database/DbProcedure.h"
#include "Database/DbNotes.h"

DetailedStatusPresenter::DetailedStatusPresenter(const Tooth& tooth, long long patientRowId)
	: m_tooth(tooth), m_checkModel(tooth), patientRowId(patientRowId), view(nullptr)
{
	m_notes = DbNotes::getNote(patientRowId, tooth.index);
}

void DetailedStatusPresenter::setView(IDetailedStatusView* view)
{
	this->view = view; 

	view->setHistoryData(DbProcedure::getToothProcedures(patientRowId, m_tooth.index));
	
	view->disableItem(StatusCode::Bridge, !m_tooth.bridge.exists());
	view->disableItem(StatusCode::FiberSplint, !m_tooth.splint.exists());
	view->disableItem(StatusCode::Temporary, m_tooth.type == ToothType::Molar);

	view->setCheckModel(m_checkModel);
	view->paintTooth(ToothHintCreator::getToothHint(m_tooth));

	view->setNotes(m_notes);
}


void DetailedStatusPresenter::stateChanged()
{
	view->paintTooth(ToothHintCreator::getToothHint(m_tooth));
}

void DetailedStatusPresenter::checkStateChanged(bool checked)
{
	m_tooth.setStatus(m_category, m_code, checked);
	m_checkModel = CheckModel(m_tooth);

	view->setCheckModel(m_checkModel);
	view->paintTooth(ToothHintCreator::getToothHint(m_tooth));
}


void DetailedStatusPresenter::statusSelected(int category, int code)
{

	m_code = code;
	m_category = static_cast<StatusType>(category);
}

void DetailedStatusPresenter::okPressed()
{
	stateChanged();
	
	_result = m_tooth;
	
	m_notes = view->getNotes();

	DbNotes::saveNote(m_notes, patientRowId, m_tooth.index);
}

std::optional<Tooth> DetailedStatusPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return _result;
}


