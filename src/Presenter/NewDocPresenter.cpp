#include "NewDocPresenter.h"
#include "Database/DbBrowser.h"
#include "View/Widgets/NewDocDialog.h"
#include "Database/DbPatient.h"
#include "PatientDialogPresenter.h"
#include "Model/User.h"

NewDocPresenter::NewDocPresenter(const std::string& title) :
	title(title)
{
}

void NewDocPresenter::newPatient()
{
	PatientDialogPresenter p;

	if (view) {
		view->close();
	}

    result = p.getPatient();
}

void NewDocPresenter::patientSelected(long long rowid)
{
	result = DbPatient::get(rowid);

	if (result && view) {
		view->close();
	}
}

std::optional<Patient> NewDocPresenter::open()
{
	auto [rows, table] = DbBrowser::getData(TabType::PatientSummary, Date(), Date());

	table.data.insert(table.data.begin(), PlainColumn());

	for (size_t i = 0; i < rows.size(); i++)
	{
		table.addCell(0, { std::to_string(rows[i].rowID) });
	}

	//if there are no patients in database
	if (rows.empty()) {

		PatientDialogPresenter p(title);

		return p.getPatient();
	}

	NewDocDialog d(*this);

	view = &d;

	d.setTable(table);

	d.setWindowTitle(title.c_str());

	d.exec();

	return result;
}
