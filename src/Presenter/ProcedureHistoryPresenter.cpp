#include "ProcedureHistoryPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"
#include "View/Widgets/ProcedureHistoryDialog.h"

ProcedureHistoryPresenter::ProcedureHistoryPresenter(const Patient& p) :
	pis_history(p.PISHistory), 
	//his_history(p.HISHistory),
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

			view->setSnapshots(snapshots);
		
		}
	);
}

bool ProcedureHistoryPresenter::refreshHospitalizations()
{
	return eHospitalizationFetch.sendRequest(ref_patient,
		User::practice().rziCode,
		[&](const std::vector<Hospitalization>& hList) {

			hospitalizations = hList;
			view->setHospitalizations(hList);

		}
	);
}


void ProcedureHistoryPresenter::setView(ProcedureHistoryDialog* v)
{
	if (v == nullptr) return;

	view = v;

	if (pis_history) view->setPis(pis_history.value());

	if (his_history) view->setHis(his_history.value());

	view->focusTab(User::hasNhifContract() ? 0 : 2);
	
}

void ProcedureHistoryPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(*this);
}

void ProcedureHistoryPresenter::pisApplyClicked()
{
	if (!pis_history) return;

	m_result.applyPis = true;

	view->closeDialog();
}

void ProcedureHistoryPresenter::statusApplyClicked()
{
	if (view->getSnapshotViewer()->getCurrentSnapshot() == nullptr) {
		return;
	}

	if (!view->getSnapshotViewer()->isMostRecent()) {

		bool answer = ModalDialogBuilder::askDialog(
			"Статусът който сте избрали не е най-актуалния."
			" Желаете ли да го приложите въпреки това?"
		);

		if (!answer) return;
	}

	m_result.statusToBeApplied = view->getSnapshotViewer()->getCurrentSnapshot()->teeth;

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
			if (view->getSnapshotViewer()->getCurrentSnapshot() == nullptr) {
				hasHSM = refreshStatus();
			}
			break;
		case 3:
			if (hospitalizations.empty()) {
				hasHSM = refreshHospitalizations();
			}
	}
}

ProcedureHistoryPresenter::Result ProcedureHistoryPresenter::result()
{

	m_result.pis_history = pis_history;
	m_result.his_history = his_history;

	return m_result;
}
