#pragma once
#include "View/Interfaces/IDetailedStatusView.h"
#include "Presenter/CheckState.h"
#include "Model/Dental/Tooth.h"
#include <optional>
#include <memory>
#include "StatusController.h"
#include "Database/DbNotes.h"


class DetailedStatusPresenter
{

	IDetailedStatusView* view{ nullptr };

	StatusType m_category{ StatusType::general };
	int m_code{ -1 };

	CheckModel m_checkModel;

	std::unique_ptr<DetailedStatusController> controller;
	

	long long patientRowId;

	Tooth m_tooth;
	std::string m_notes;

	std::optional<Tooth> _result{};

public:
	DetailedStatusPresenter(const Tooth& tooth, long long patientRowId);

	void setView(IDetailedStatusView* view);
	void checkStateChanged(bool checked);
	void stateChanged();
	void statusSelected(int category, int code);
	const std::string& getNote() const { return m_notes; }

	void okPressed();

	std::optional<Tooth> open();

	void setDynamicDisable();

};
