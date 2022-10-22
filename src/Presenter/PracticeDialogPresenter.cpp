#include "PracticeDialogPresenter.h"
#include "Database/Database.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPractice.h"
#include <fstream>
#include <Model/Parser.h>


PracticeDialogPresenter::PracticeDialogPresenter(const std::string& practiceRzi) :
	m_practicePresenter(practiceRzi)
{

}


void PracticeDialogPresenter::setView(IAddPracticeDialog* view)
{
	this->view = view;

	m_practicePresenter.setView(view->practiceSettingsView());
}

std::optional<PracticeWithDoctors> PracticeDialogPresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return m_result;
}

void PracticeDialogPresenter::okPressed()
{
	if (!m_practicePresenter.isValid()) return;

	m_result.emplace(
		PracticeWithDoctors{
			m_practicePresenter.getPractice(),
			m_practicePresenter.getDoctorsList()
		}
	);

	view->closeDialog();
}
