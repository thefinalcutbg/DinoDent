#pragma once
#include "View/DetailsView/IDetailedStatusView.h"
#include "Presenter/ListPresenter/StatusPresenter/CheckState.h"
#include "Model/Tooth/Tooth.h"
#include <optional>
#include <memory>
#include "StatusController.h"
#include "Database/DbNotes.h"


class DetailedStatusPresenter
{
	DbNotes db_notes;

	StatusType m_category{ StatusType::general };
	int m_code{ -1 };

	CheckModel m_checkModel;

	std::unique_ptr<DetailedStatusController> controller;
	
	IDetailedStatusView* view{nullptr};

	const std::string& patientID;

	Tooth m_tooth;
	std::string m_notes;

	std::optional<Tooth> _result{};

public:
	DetailedStatusPresenter(const Tooth& tooth, const std::string& patientID);

	void setView(IDetailedStatusView* view);
	void checkStateChanged(bool checked);
	void stateChanged();
	void statusSelected(int category, int code);

	void okPressed();

	std::optional<Tooth> open();

	void setDynamicDisable();

};

