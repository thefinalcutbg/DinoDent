#include "DetailedStatusPresenter.h"

#include "Presenter/CheckModel.h"
#include "Database/DbNotes.h"
#include "View/Graphics/PaintHint.h"
#include "View/ModalDialogBuilder.h"

DetailedStatusPresenter::DetailedStatusPresenter(const Tooth& tooth, long long patientRowId, const std::vector<Procedure>& toothProcedures)
    : view(nullptr),
    m_procedures(toothProcedures),
    m_checkModel(tooth),
    m_dsnCheckModel(tooth.dsn.tooth()),
    patientRowId(patientRowId),
    m_tooth(tooth)
{
	m_notes = DbNotes::getNote(patientRowId, tooth.index);
}

void DetailedStatusPresenter::setView(IDetailedStatusView* view)
{
	this->view = view; 

	view->setHistoryData(m_procedures);
	
	view->disableItem(StatusCode::Bridge, !m_tooth.bridge.exists());
	view->disableItem(StatusCode::FiberSplint, !m_tooth.splint.exists());
	view->disableItem(StatusCode::Temporary, m_tooth.type == ToothType::Molar);

	view->setCheckModel(m_checkModel, m_dsnCheckModel);
	view->paintTooth(ToothPaintHint(m_tooth));
	view->setNotes(m_notes);
}


void DetailedStatusPresenter::stateChanged()
{
	view->paintTooth(ToothPaintHint(m_tooth));
}

void DetailedStatusPresenter::checkStateChanged(bool checked)
{
	auto& tooth = m_supernumeral ? m_tooth.dsn.tooth() : m_tooth;
	tooth.setStatus(m_category, m_code, checked);

	m_checkModel = CheckModel(m_tooth);
	m_dsnCheckModel = CheckModel(m_tooth.dsn.tooth());

	view->setCheckModel(m_checkModel, m_dsnCheckModel);
	view->paintTooth(ToothPaintHint(m_tooth));
}


void DetailedStatusPresenter::statusSelected(int category, int code, bool on_supernumeral)
{
	this->m_supernumeral = on_supernumeral;
	m_code = code;
	m_category = static_cast<StatusType>(category);
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
	stateChanged();
	
	_result = m_tooth;
	
	m_notes = view->getNotes();

	DbNotes::saveNote(m_notes, patientRowId, m_tooth.index);
}

std::optional<Tooth> DetailedStatusPresenter::open()
{
    ModalDialogBuilder::openDialog(*this);

	return _result;
}


