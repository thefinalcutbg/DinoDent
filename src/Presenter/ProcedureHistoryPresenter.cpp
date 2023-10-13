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

	return pis_service.sendRequest(ref_patient, true,
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
	return dental_history_service.sendRequest(ref_patient,
		[&](const std::vector<HisSnapshot>& snapshots) {

			if (snapshots.empty()) {
				ModalDialogBuilder::showMessage(
					"За този пациент не са намерени данни в НЗИС"
				);
				return;
			}

			status_snapshots = snapshots;
			view->setSliderCount(snapshots.size()-1);
			view->setSliderIndex(snapshots.size()-1);
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
	if (status_snapshots.empty()) return;

	if (current_snapshot != status_snapshots.size() - 1) {

		bool answer = ModalDialogBuilder::askDialog(
			"Статусът който сте избрали не е най-актуалния."
			" Желаете ли да го приложите въпреки това?"
		);

		if (!answer) return;
	}

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
			if (status_snapshots.empty()) {
				hasHSM = refreshStatus();
			}
			break;
	}
}

void ProcedureHistoryPresenter::sliderPositionChanged(int position)
{
	if (position < 0 || position > status_snapshots.size()) return;

	current_snapshot = position;

	view->setSnapshot(status_snapshots[position]);
}


ProcedureHistoryPresenter::Result ProcedureHistoryPresenter::result()
{
	Result result;

	result.pis_history = pis_history;
	result.his_history = his_history;

	if (m_applyStatus) {
		result.statusToBeApplied = status_snapshots.size() ?
			status_snapshots[current_snapshot].teeth
			:
			ToothContainer();
	}

	result.applyPis = m_applyPis;

	return result;
}
