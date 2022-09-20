#include "AddPracticePresenter.h"
#include "Database/Database.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPractice.h"
#include <fstream>
#include <Model/Parser.h>

void AddPracticePresenter::setView(IAddPracticeDialog* view)
{
	this->view = view;

	m_practicePresenter.setView(view->practiceSettingsView());
	m_doctorsPresenter.setView(view->doctorSettingsView());
}

std::optional<NewPractice> AddPracticePresenter::open()
{
	ModalDialogBuilder::openDialog(this);

	return m_result;
}

void AddPracticePresenter::okPressed()
{
	if (!m_practicePresenter.isValid()) return;

	if (!m_doctorsPresenter.isValid())
	{
		ModalDialogBuilder::showError(u8"Практиката трябва да има поне един администратор!");
		return;
	}
		
	m_result.emplace(
		NewPractice{
			m_practicePresenter.getPractice(),
			m_doctorsPresenter.getDoctorsList()
		}
	);
	
	if (DbPractice::practiceExists(m_result.value().practice.rziCode)) {
		m_result.reset();
		ModalDialogBuilder::showError(u8"Практика с такъв РЗИ номер вече съществува!");
		return;
	}


	view->closeDialog();
}
