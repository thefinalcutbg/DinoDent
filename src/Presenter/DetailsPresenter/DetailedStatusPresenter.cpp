#include "DetailedStatusPresenter.h"
#include "Presenter/ListPresenter/CheckState.h"
#include "Presenter/ListPresenter/ToothHintCreator.h"
#include "View/ModalDialogBuilder.h"
#include "StatusController.h"
#include "Database/DbProcedure.h"
#include "Database/DbNotes.h"

DetailedStatusPresenter::DetailedStatusPresenter(const Tooth& tooth, long long patientRowId)
	: m_tooth(tooth), m_checkModel(tooth), patientRowId(patientRowId), controller{nullptr}, view(nullptr)
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
}


void DetailedStatusPresenter::stateChanged()
{
	if (controller) controller->applyChange();
	view->paintTooth(ToothHintCreator::getToothHint(m_tooth));
}

void DetailedStatusPresenter::checkStateChanged(bool checked)
{
	m_tooth.setStatus(m_category, m_code, checked);
	m_checkModel = CheckModel(m_tooth);

	view->setCheckModel(m_checkModel);
	view->paintTooth(ToothHintCreator::getToothHint(m_tooth));

	setDynamicDisable();
}


void DetailedStatusPresenter::statusSelected(int category, int code)
{
	if (controller) controller->applyChange();

	m_code = code;

	view->clearData();


	switch (category)
	{
	case 0:
	{
		m_category = StatusType::general;

		switch (m_code)
		{
			case StatusCode::Obturation:
				controller = std::make_unique<ObturationBulk>(*view, m_tooth.obturation); break;
			case StatusCode::Caries:
				controller = std::make_unique<CariesBulk>(*view, m_tooth.caries); break;
			case StatusCode::Pulpitis:
				controller = std::make_unique<PathologyControl>(*view, m_tooth.pulpitis); break;
			case StatusCode::ApicalLesion:
				controller = std::make_unique <PathologyControl>(*view, m_tooth.lesion); break;
			case StatusCode::Fracture:
				controller = std::make_unique <PathologyControl>(*view, m_tooth.fracture); break;
			case StatusCode::Crown:
				controller = std::make_unique<CrownControl>(*view, m_tooth.crown); break;
			case StatusCode::Root:
				controller = std::make_unique<PathologyControl>(*view, m_tooth.root); break;
			case StatusCode::EndoTreatment:
				controller = std::make_unique<DentistMadeControl>(*view, m_tooth.endo); break;
			case StatusCode::Extraction:
				controller = std::make_unique<DentistMadeControl>(*view, m_tooth.extraction); break;
			case StatusCode::Post:
				controller = std::make_unique<DentistMadeControl>(*view, m_tooth.post); break;
			case StatusCode::Bridge:
				controller = std::make_unique<CrownControl>(*view, m_tooth.bridge); break;
			case StatusCode::FiberSplint:
				controller = std::make_unique<FiberSplintControl>(*view, m_tooth.splint); break;
			case StatusCode::Implant:
				controller = std::make_unique<ImplantControl>(*view, m_tooth.implant); break;
			default: break;
		}

		break;
	}
	case 1:
		m_category = StatusType::obturation;
		controller = std::make_unique<ObturationControl>(*view, m_tooth.obturation[m_code]); break;
	case 2:
		m_category = StatusType::caries;
		controller = std::make_unique <PathologyControl>(*view, m_tooth.caries[m_code]); break;
	case 3:
		controller = std::make_unique<NotesControl>(*view, m_notes);
		view->disableDetails(false); return; //avoiding dynamicDisable
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

void DetailedStatusPresenter::okPressed()
{
	stateChanged();
	
	_result = m_tooth;

	DbNotes::saveNote(m_notes, patientRowId, m_tooth.index);
}

std::optional<Tooth> DetailedStatusPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return _result;
}


