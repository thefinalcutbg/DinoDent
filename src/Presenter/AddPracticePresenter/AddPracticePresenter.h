#pragma once
#include "View/AddPracticeDialog/IAddPracticeDialog.h"
#include "Presenter/SettingsPresenter/PracticeSettingsPresenter.h"
#include "Presenter/SettingsPresenter/PracticeDoctorSettingsPresenter.h"
#include "Model/User/User.h"
#include <optional>

struct NewPractice {
	Practice practice;
	std::vector<PracticeDoctor> doctorsList;
};

class AddPracticePresenter
{
	IAddPracticeDialog* view;
	
	std::optional<NewPractice> m_result;

	PracticeSettingsPresenter m_practicePresenter;
	PracticeDoctorSettingsPresenter m_doctorsPresenter;

public:

	std::optional<NewPractice> open();
	void setView(IAddPracticeDialog* view);
	void okPressed();
	
};