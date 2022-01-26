#include "PatientSummaryPresenter.h"
#include "View/TabView/ITabView.h"
#include "View/PatientSummaryView/IPatientSummaryView.h"

PatientSummaryPresenter::PatientSummaryPresenter(ITabView* view, std::shared_ptr<Patient> patient)
    :   TabInstance(view, TabType::PatientSummary), 
        patient(patient), 
        view(view->summaryView())
{}




void PatientSummaryPresenter::print()
{
}


void PatientSummaryPresenter::setCurrent()
{
    view->setPresenter(this);
    _tabView->showSummaryView();
   
}



std::string PatientSummaryPresenter::getTabName()
{
    return std::string();
}
