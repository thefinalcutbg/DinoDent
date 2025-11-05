#include "DetailedStatusPresenter.h"

#include "Presenter/CheckModel.h"
#include "Database/DbNotes.h"
#include "View/Graphics/PaintHint.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/DetailedStatus.h"

DetailedStatusPresenter::DetailedStatusPresenter(int tooth_index ,long long patientRowId, const std::vector<Procedure>& toothProcedures)
    : view(nullptr),
    m_procedures(toothProcedures),
    patientRowId(patientRowId),
    m_tooth_index(tooth_index)
{
	m_notes = DbNotes::getNote(patientRowId, tooth_index);
}

void DetailedStatusPresenter::setView(DetailedStatus* view)
{
	this->view = view; 
	
	view->setNotes(m_notes);

	bool focusNotes = m_notes.size() || m_procedures.empty();

	view->focusNotes(focusNotes);

	for (auto& p : m_procedures)
	{
		if (p.db_source == Procedure::DatabaseSource::PIS){
			tableOptionChanged(false, false, true);
			return;
		}
	}

	for (auto& p : m_procedures)
	{
		if (p.db_source == Procedure::DatabaseSource::HIS){
			tableOptionChanged(false, true, false);
		return;
		}
	}

	for (auto& p : m_procedures)
	{
		if (p.db_source == Procedure::DatabaseSource::Local) {
			tableOptionChanged(true, false, false);
			return;
		}
	}
}

void DetailedStatusPresenter::tableOptionChanged(bool local, bool his, bool pis)
{
	std::vector<Procedure> toDisplay;

	toDisplay.reserve(m_procedures.size());
	
	auto requestedSource = 
		local ? Procedure::DatabaseSource::Local :
		his ? Procedure::DatabaseSource::HIS :
		pis ? Procedure::DatabaseSource::PIS :
		Procedure::DatabaseSource::UnknownSource;

	for (auto& p : m_procedures) {

		if (p.db_source != requestedSource){
			continue;
		}

		toDisplay.push_back(p);
	}

	view->setHistoryData(toDisplay, requestedSource);
}

void DetailedStatusPresenter::okPressed()
{
	
	m_notes = view->getNotes();

	DbNotes::saveNote(m_notes, patientRowId, m_tooth_index);
}

void DetailedStatusPresenter::open(Procedure::DatabaseSource s)
{
	DetailedStatus d(*this);

	if(s != Procedure::DatabaseSource::UnknownSource){

		view->focusNotes(false);

		tableOptionChanged(
			s == Procedure::DatabaseSource::Local,
			s == Procedure::DatabaseSource::HIS,
			s == Procedure::DatabaseSource::PIS
		);
	}

	d.exec();
}


