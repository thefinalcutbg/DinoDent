#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/TimeFrame.h"
#include <memory>
#include "View/Theme.h"

class PatientSummaryView;
struct Patient;
class TabPresenter;

class PatientSummaryPresenter : public TabInstance
{

	IPatientSummaryView* view;
	TabPresenter* tab_presenter;
	int m_currentFrameIdx;
	std::vector<TimeFrame> statusTimeFrame;
	int m_selectedTooth{ -1 };


	//a function for convinience
	TimeFrame* currentFrame();

public:

	
	PatientSummaryPresenter(ITabView* view, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient);

	void setCurrentFrame(int index);
	void openPatientDialog();
	void openAllergiesDialog();

	long long rowID() const override;
	bool save() override { return true; };
	bool saveAs() override { return true; };
	bool isNew() override { return false; };
	void openCurrentDocument();

	void print() override;
	void setCurrent() override;
	void toothSelected(int toothIdx);
	virtual TabName getTabName() override;

	~PatientSummaryPresenter();
};

