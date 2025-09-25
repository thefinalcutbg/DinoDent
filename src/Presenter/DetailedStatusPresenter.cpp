#include "DetailedStatusPresenter.h"

#include "Presenter/CheckModel.h"
#include "Database/DbNotes.h"
#include "View/Graphics/PaintHint.h"
#include "View/ModalDialogBuilder.h"

DetailedStatusPresenter::DetailedStatusPresenter(int tooth_index ,long long patientRowId, const std::vector<Procedure>& toothProcedures)
    : view(nullptr),
    m_procedures(toothProcedures),
    patientRowId(patientRowId),
    m_tooth_index(tooth_index)
{
	m_notes = DbNotes::getNote(patientRowId, tooth_index);
}

void DetailedStatusPresenter::setView(IDetailedStatusView* view)
{
	this->view = view; 

	view->setHistoryData(m_procedures);
	
	view->setNotes(m_notes);

	bool focusNotes = m_notes.size() || m_procedures.empty();

	view->focusNotes(focusNotes);

	if (focusNotes) return;


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

	for (auto& p : m_procedures) {

		switch (p.db_source)
		{
			case Procedure::DatabaseSource::Local: if (!local) continue; break;
			case Procedure::DatabaseSource::HIS: if (!his) continue; break;
			case Procedure::DatabaseSource::PIS: if (!pis) continue; break;
		}

		toDisplay.push_back(p);
	}

	view->setHistoryData(toDisplay);
}

void DetailedStatusPresenter::okPressed()
{
	
	m_notes = view->getNotes();

	DbNotes::saveNote(m_notes, patientRowId, m_tooth_index);
}

void DetailedStatusPresenter::open()
{
    ModalDialogBuilder::openDialog(*this);
}


