#include "UnusedPackagePresenter.h"
#include "Database/DbPatient.h"
#include "Database/DbProcedure.h"
#include "View/Widgets/UnusedPackageView.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "TabPresenter.h"
#include "Model/TableRows.h"

void UnusedPackagePresenter::stop(const std::string& reason)
{
	if (reason.size()) {
		ModalDialogBuilder::showMessage(reason);
	}

	m_in_progress = false;

	view->reset();
}

void UnusedPackagePresenter::resetQueue()
{
	m_queue = std::queue<Patient>{};
}

void UnusedPackagePresenter::newAmbList(long long patientRowid)
{
	RowInstance instance(TabType::AmbList);
	instance.rowID = 0;
	instance.patientRowId = patientRowid;
	TabPresenter::get().open(instance, true);
}

void UnusedPackagePresenter::popQueue()
{
	if (m_queue.size()) {
		m_queue.pop();
		view->increment();
	}
}

UnusedPackagePresenter::UnusedPackagePresenter(){}

void UnusedPackagePresenter::setView(UnusedPackageView* view)
{
	this->view = view;
	view->addTable(s_data);
}

void UnusedPackagePresenter::buttonPressed(const Date& excludeBefore)
{
	if (m_in_progress) {
		stop();
		return;
	}

	if (!User::hasNhifContract() ||
		User::practice().nhif_contract->nra_pass.empty()
	) 
	{
		ModalDialogBuilder::showMessage("Не е въведена парола за НАП");
		return;
	}

	if (m_queue.empty()) {
		s_data.clear();
		view->addTable(s_data);
		m_queue = DbPatient::getPatientList(excludeBefore, User::practice().rziCode, User::doctor().LPK);
	}

	m_in_progress = true;

	view->setProgressCount(m_queue.size());

	step1_localDbCheck();

}

void UnusedPackagePresenter::step1_localDbCheck()
{
	while (m_queue.size() && m_in_progress) {

		auto& patient = m_queue.front();

		auto summary = DbProcedure::getNhifSummary(
			patient.rowid,
			0,
			Date(1, 1, m_year),
			Date(31, 12, m_year)
		);

		int procedure_counter = 0;

		for (auto &p : summary) {

			if (code_set.count(p.code)) {
				procedure_counter++;
			}
		}

		int max_procedures = patient.isAdult() ? 3 : 4;

		if (procedure_counter >= max_procedures) {
			popQueue();
			continue;
		}
		else
		{
			nraService.sendRequest(
				patient,
				[&](auto result) { this->step2_insuranceCheck(result); },
				false
			);

			return;
			
		}
		
	}

	stop();
	
}

void UnusedPackagePresenter::step2_insuranceCheck(const std::optional<InsuranceStatus>& status)
{
	if (!status.has_value()) {
		stop("Възникна грешка при свързване с НАП. Опитайте отново");
		return;
	};

	if (status.value().status != Insured::Yes) {
		popQueue();
		step1_localDbCheck();
		return;
	}

	bool requestSent = pisService.sendRequest(
		m_queue.front(),
		false,
		[&](auto result) {
			this->step3_pisCheck(result);
		});

	if (!requestSent) {
		ModalDialogBuilder::showMessage("Не е открит КЕП");
		stop();
	}
}

void UnusedPackagePresenter::step3_pisCheck(const std::optional<std::vector<Procedure>>& pisHistory)
{
	if (!pisHistory) {
		stop("Възникна грешка при свързване с ПИС. Опитайте отново");
		return;
	}

	auto& patient = m_queue.front();

	std::vector<ProcedureSummary> procedures;

	Date maxDate;
	Date upperDenture;
	Date lowerDenture;

	for (auto& p : pisHistory.value()) {

		if (p.code.oldCode() == 831) {
			upperDenture = std::max(upperDenture, p.date);
		}

		if (p.code.oldCode() == 832) {
			lowerDenture = std::max(lowerDenture, p.date);
		}

		if (p.date.year != m_year) continue;

		procedures.push_back(ProcedureSummary{
					.date = p.date,
					.code = p.code.oldCode(),
					.tooth_idx = p.tooth_idx
			});

		maxDate = std::max(maxDate, p.date);
	}

	auto dbProcedures = DbProcedure::getNhifSummary(
		patient.rowid,
		0,
		maxDate,
		Date(31, 12, m_year)
	);

	procedures.insert(procedures.end(), dbProcedures.begin(), dbProcedures.end());

	int max_procedures = patient.isAdult() ? 3 : 4;

	int procedure_counter = 0;
	bool exam = false;

	for (auto& p : procedures) {

		if (p.code == 832) {
			upperDenture = std::max(upperDenture, p.date);
		}

		if (p.code == 833) {
			lowerDenture = std::max(lowerDenture, p.date);
		}

		if (p.date.year != m_year) continue;

		if (p.code == 101) {
			exam = true;
		}

		if (code_set.count(p.code)) {
			procedure_counter++;
		}
	}

	if (procedure_counter >= max_procedures) {
		popQueue();
		step1_localDbCheck();
		return;
	}

	auto lastVisit = DbPatient::getLastVisit(
		patient.rowid,
		User::practice().rziCode,
		User::doctor().LPK
	);

	auto row = PackageRowData{
		patient.rowid,
		patient.firstLastName(),
		patient.getAge(),
		patient.phone,
		lastVisit == Date() ? "Няма" : lastVisit.to8601(),
		exam,
		procedure_counter,
		max_procedures,
		upperDenture == Date() ? "" : upperDenture.toBgStandard(),
		lowerDenture == Date() ? "" : lowerDenture.toBgStandard()
	};

	s_data.push_back(row);

	view->addRow(row);

	popQueue();
	step1_localDbCheck();
}
