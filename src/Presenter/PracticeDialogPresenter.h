#pragma once
#include "View/Interfaces/IPracticeDialog.h"
#include "Presenter/PracticeSettingsPresenter.h"
#include "Model/UserStructs.h"
#include <optional>

struct PracticeWithDoctors {
	Practice practice;
	std::vector<PracticeDoctor> doctorsList;
};

class PracticeDialogPresenter
{
	IAddPracticeDialog* view{ nullptr };
	
	std::optional<PracticeWithDoctors> m_result;

	PracticeSettingsPresenter m_practicePresenter;

public:
	//default empty string for adding new practice
	PracticeDialogPresenter(const std::string& practiceRzi = std::string{});
	std::optional<PracticeWithDoctors> open();
	void setView(IAddPracticeDialog* view);
	void okPressed();
	
};