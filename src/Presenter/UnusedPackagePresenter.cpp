#include "UnusedPackagePresenter.h"
#include "Database/DbPatient.h"
#include "Database/DbProcedure.h"
#include "View/Widgets/UnusedPackageView.h"
#include "Model/User.h"

void UnusedPackagePresenter::popQueue()
{
	if (m_queue.size()) {
		m_queue.pop();
		view->increment();
	}
}

UnusedPackagePresenter::UnusedPackagePresenter(UnusedPackageView* view) :
	view(view)
{}

void UnusedPackagePresenter::generateReport(const Date& date)
{
	m_queue = DbPatient::getPatientList(date, User::practice().rziCode, User::doctor().LPK);

	view->setProgressCount(m_queue.size());

	step1_localDbCheck();

}

void UnusedPackagePresenter::step1_localDbCheck()
{
	while (m_queue.size()) {

		auto& patient = m_queue.front().second;

		auto summary = DbProcedure::getNhifSummary(
			m_queue.front().second.rowid,
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

		int max_procedures = m_queue.front().second.isAdult() ? 3 : 4;

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
	
}

void UnusedPackagePresenter::step2_insuranceCheck(const std::optional<InsuranceStatus>& status)
{
	if (!status.has_value()) {
		view->appendText("Възникна грешка при свързване с НАП. Опитайте отново");
		return;
	};

	if (status.value().status != Insured::Yes) {
		popQueue();
		step1_localDbCheck();
		return;
	}

	pisService.sendRequest(
		m_queue.front().second,
		false,
		[&](auto result) {
			this->step3_pisCheck(result);
		});
}

void UnusedPackagePresenter::step3_pisCheck(const std::optional<std::vector<Procedure>>& pisHistory)
{
	if (!pisHistory) {
		view->appendText("Възникна грешка при свързване с ПИС. Опитайте отново");
		return;
	}

	auto& patient = m_queue.front().second;

	std::vector<ProcedureSummary> procedures;

	Date maxPisDate = Date(1, 1, m_year);

	for (auto& p : pisHistory.value()) {

		if (p.date.year != m_year) continue;

		maxPisDate = std::max(maxPisDate, p.date);

		procedures.push_back(ProcedureSummary{
					.date = p.date,
					.code = p.code.oldCode(),
					.tooth_idx = p.tooth_idx
			});

		maxPisDate = std::max(maxPisDate, p.date);
	}

	auto dbProcedures = DbProcedure::getNhifSummary(
		patient.rowid,
		0,
		maxPisDate,
		Date(31, 12, m_year)
	);

	procedures.insert(procedures.end(), dbProcedures.begin(), dbProcedures.end());

	int max_procedures = m_queue.front().second.isAdult() ? 3 : 4;

	int procedure_counter = 0;
	bool exam = false;

	for (auto& p : procedures) {
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

	std::string data = patient.firstLastName();
	data +="\t\t ";
	data += "Преглед за годината: ";
	data += exam ? "Да " : "Не ";
	data += "\t\tЛимит на НЗОК процедури: ";
	data += std::to_string(procedure_counter);
	data += "/";
	data += std::to_string(max_procedures);

	view->appendText(data);

	popQueue();
	step1_localDbCheck();
}
