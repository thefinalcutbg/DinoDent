#pragma once
#include "View/Interfaces/IAddPracticeDialog.h"
#include "Presenter/PracticeSettingsPresenter.h"
#include "Presenter/PracticeDoctorSettingsPresenter.h"
#include "Model/UserStructs.h"
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