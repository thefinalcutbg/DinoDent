#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/TimeFrame.h"
#include <memory>

class PatientSummaryView;
struct Patient;


class PatientSummaryPresenter final : public TabInstance
{

	IPatientSummaryView* view;

	int m_currentFrameIdx;

public:
	std::vector<TimeFrame> statusTimeFrame;
	

	PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient);

	void setCurrentFrame(int index);

	long long rowID() const override;
	bool save() override { return true; };
	bool saveAs() override { return true; };
	bool isNew() override { return false; };


	void print() override;
	void setCurrent() override;

	virtual TabName getTabName() override;

	~PatientSummaryPresenter();
};

