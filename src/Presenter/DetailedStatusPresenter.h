#pragma once

#include <optional>
#include <vector>

#include "Model/Patient.h"
#include "Model/Dental/Procedure.h"

class DetailedStatus;

class DetailedStatusPresenter
{
	DetailedStatus* view{ nullptr };

	std::vector<Procedure> m_procedures;

	const Patient& m_patient;

	int m_tooth_index;
	std::string m_notes;

	std::optional<Tooth> _result{};

public:

	DetailedStatusPresenter(int toothIdx, const Patient& patient);

	void setView(DetailedStatus* view);
	void tableOptionChanged(bool local, bool his, bool pis);
	const std::string& getNote() const { return m_notes; }

	void okPressed();

	void open(Procedure::DatabaseSource s = Procedure::UnknownSource);
};

