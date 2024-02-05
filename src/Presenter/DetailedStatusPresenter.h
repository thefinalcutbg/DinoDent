#pragma once
#include "View/Interfaces/IDetailedStatusView.h"
#include "Presenter/CheckModel.h"
#include "Model/Dental/Tooth.h"
#include <optional>

class DetailedStatusPresenter
{

	IDetailedStatusView* view{ nullptr };

	std::vector<Procedure> m_procedures;

	CheckModel m_checkModel;
	CheckModel m_dsnCheckModel;

	long long patientRowId;

	int m_tooth_index;
	std::string m_notes;

	std::optional<Tooth> _result{};

public:
	DetailedStatusPresenter(int toothIdx, long long patientRowId, const std::vector<Procedure>& toothProcedures);

	void setView(IDetailedStatusView* view);
	void tableOptionChanged(bool local, bool his, bool pis);
	const std::string& getNote() const { return m_notes; }

	void okPressed();

	void open();
};

