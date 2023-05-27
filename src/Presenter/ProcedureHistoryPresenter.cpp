#include "ProcedureHistoryPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"

ProcedureHistoryPresenter::ProcedureHistoryPresenter(const Patient& p) :
	pis_history(p.PISHistory), 
	his_history(p.HISHistory),
	ref_patient(p)
{
}

void ProcedureHistoryPresenter::refreshPIS()
{
	if (!User::hasNzokContract()) {
		ModalDialogBuilder::showMessage("Този доктор няма договор със здравната каса!");
		return;
	}

	pis_service.sendRequest(ref_patient.type, ref_patient.id, true,
		[&](const std::optional<std::vector<Procedure>> pis_data)
		{
			if (!pis_data.has_value()) return;

			if (pis_data->empty()) {
				ModalDialogBuilder::showMessage("За този пациент не са открити данни в ПИС");
			}

			pis_history = pis_data;
			view->setPis(pis_history.value());
		}
	);
	
}

void ProcedureHistoryPresenter::refreshHIS()
{
	
	his_service.sendRequest(ref_patient, true,
		[&](const std::optional<std::vector<Procedure>> his_data)
		{
			if (!his_data.has_value()) return;
			
			if (his_data->empty()) {
				ModalDialogBuilder::showMessage("За този пациент не са открити данни в НЗИС");
			}
			
			his_history = his_data;
			view->setHis(his_history.value());
			
		}
	);
}

void ProcedureHistoryPresenter::refreshStatus()
{
	status_service.sendRequest(ref_patient,
		[&](const ToothContainer& teeth) {
			current_status = teeth;
			view->setCurrentStatus(teeth);
		}
	);
}

void ProcedureHistoryPresenter::setView(IProcedureHistoryDialog* v)
{
	if (v == nullptr) return;

	view = v;

	if (pis_history) 
	{
		view->setPis(pis_history.value());
	}
	else
	{
		if(User::hasNzokContract())
		{ 
			refreshPIS(); 
		}
		else 
		{
			view->focusHisTab();
		}
	}


	if (his_history) {
		view->setHis(his_history.value());
	}
	else
	{
		refreshHIS();
	}

	status_service.sendRequest(ref_patient,
		[&](const ToothContainer& teeth) {
			current_status = teeth;
			view->setCurrentStatus(teeth);
		}
	);
}

void ProcedureHistoryPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(*this);
}

void ProcedureHistoryPresenter::pisApplyClicked()
{
	if (!pis_history) return;

	m_applyPis = true;

	view->closeDialog();
}

void ProcedureHistoryPresenter::statusApplyClicked()
{
	if (!current_status) return;

	m_applyStatus = true;

	view->closeDialog();
}

ProcedureHistoryPresenter::Result ProcedureHistoryPresenter::result()
{
	return ProcedureHistoryPresenter::Result{
		.pis_history = pis_history,
		.his_history = his_history,
		.current_status = current_status,
		.applyPis = m_applyPis,
		.applyCurrentStatus = m_applyStatus
	};
}
