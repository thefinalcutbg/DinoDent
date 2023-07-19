#include "ProcedureHistoryPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"

ProcedureHistoryPresenter::ProcedureHistoryPresenter(const Patient& p) :
	pis_history(p.PISHistory), 
	his_history(p.HISHistory),
	ref_patient(p)
{
}

bool ProcedureHistoryPresenter::refreshPIS()
{
	if (!User::hasNhifContract()) {
		ModalDialogBuilder::showMessage("Този доктор няма договор със здравната каса!");
		return true;
	}

	return pis_service.sendRequest(ref_patient.type, ref_patient.id, true,
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

bool ProcedureHistoryPresenter::refreshHIS()
{
	
	return his_service.sendRequest(ref_patient, true,
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

bool ProcedureHistoryPresenter::refreshStatus()
{
	return status_service.sendRequest(ref_patient,
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

	if (pis_history) view->setPis(pis_history.value());

	if (his_history) view->setHis(his_history.value());

	view->focusTab(User::hasNhifContract() ? 0 : 1);
	
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

void ProcedureHistoryPresenter::tabFocused(int idx)
{
	if (!hasHSM) return;

	if (tabIdxFirstFocus[idx] == false) return;

	tabIdxFirstFocus[idx] = false;

	switch (idx) 
	{
		case 0:
			if (User::hasNhifContract() && !pis_history.has_value()) {
				hasHSM = refreshPIS();
			}
			break;
		case 1:
			if (!his_history.has_value()) {
				hasHSM = refreshHIS();
			}
			break;
		case 2:
			if (!current_status.has_value()) {
				hasHSM = refreshStatus();
			}
			break;
	}
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
