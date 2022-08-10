#pragma once
#include "Presenter/TabPresenter/TabInstance.h"
#include "Model/TimeFrame.h"
#include <memory>
#include "View/Theme.h"
#include "View/PatientSummaryView/IPatientSummaryView.h"

struct Patient;
class TabPresenter;

class PatientSummaryPresenter : public TabInstance
{

	IPatientSummaryView* view;
	SummaryState state;

	TabPresenter* tab_presenter;

	std::vector<TimeFrame> statusTimeFrame;

	//a function for convinience
	TimeFrame* currentFrame();

public:

	
	PatientSummaryPresenter(ITabView* view, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient);

	bool noData() { return statusTimeFrame.empty(); }

	void setCurrentFrame(int index);
	void pricePeriodChanged(const Date& from, const Date& to);
	void toothSelected(int toothIdx);
	void teethViewButtonClicked(bool showBuccal);
	void perioCheckBoxClicked(bool checked);

	void openPatientDialog();
	void openAllergiesDialog();

	long long rowID() const override;
	bool save() override { return true; };
	bool isNew() override { return false; };
	void openCurrentDocument();
	void print() override;
	void setDataToView() override;

	virtual TabName getTabName() override;

	~PatientSummaryPresenter();
};

