#include "UnusedPackagePresenter.h"
#include "Database/DbPatient.h"
#include "View/Widgets/UnusedPackageView.h"
#include "Model/User.h"

UnusedPackagePresenter::UnusedPackagePresenter(UnusedPackageView* view) :
	view(view)
{}

void UnusedPackagePresenter::generateReport(const Date& date)
{
	auto patients = DbPatient::getPatientList(date, User::practice().rziCode, User::doctor().LPK);

	for (auto& [lastvisit, patient] : patients) {
		view->appendText(patient.firstLastName());
	}
}
