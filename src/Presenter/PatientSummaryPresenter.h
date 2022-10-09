#pragma once
#include "Presenter/TabInstance.h"
#include "Presenter/PatientInfoPresenter.h"
#include "Model/Dental/TimeFrame.h"
#include <memory>
#include "View/Theme.h"
#include "View/Interfaces/IPatientSummaryView.h"

struct Patient;
class TabPresenter;

class PatientSummaryPresenter : public TabInstance
{

	IPatientSummaryView* view;
	SummaryState state;
	PatientInfoPresenter patient_presenter;
	TabPresenter* tab_presenter;

	std::vector<TimeFrame> statusTimeFrame;

	//a function for convinience
	TimeFrame* currentFrame();

public:

	
	PatientSummaryPresenter(ITabView* view, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient);

	bool noData() { return statusTimeFrame.empty(); }

	void setCurrentFrame(int index);
	//void pricePeriodChanged(const Date& from, const Date& to);
	void toothSelected(int toothIdx);
	void teethViewButtonClicked(bool showBuccal);
	void perioCheckBoxClicked(bool checked);

	long long rowID() const override;
	bool save() override { return true; };
	bool isNew() override { return false; };
	void openCurrentDocument();
	void print() override;
	void setDataToView() override;

	virtual TabName getTabName() override;

	~PatientSummaryPresenter();
};

