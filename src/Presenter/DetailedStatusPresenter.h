#pragma once
#include "View/Interfaces/IDetailedStatusView.h"
#include "Presenter/CheckState.h"
#include "Model/Dental/Tooth.h"
#include <optional>
#include <memory>

#include "Database/DbNotes.h"


class DetailedStatusPresenter
{

	IDetailedStatusView* view{ nullptr };

	StatusType m_category{ StatusType::general };
	int m_code{ -1 };
	bool m_supernumeral;

	std::vector<Procedure> m_procedures;

	CheckModel m_checkModel;
	CheckModel m_dsnCheckModel;

	long long patientRowId;

	Tooth m_tooth;
	std::string m_notes;

	std::optional<Tooth> _result{};

public:
	DetailedStatusPresenter(const Tooth& tooth, long long patientRowId, const std::vector<Procedure>& toothProcedures);

	void setView(IDetailedStatusView* view);
	void checkStateChanged(bool checked);
	void stateChanged();
	void statusSelected(int category, int code, bool on_supernumeral);
	const std::string& getNote() const { return m_notes; }

	void okPressed();

	std::optional<Tooth> open();
};

