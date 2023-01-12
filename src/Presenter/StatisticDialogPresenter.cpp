#include "StatisticDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbStatistics.h"
#include "Model/Dental/Procedure.h"

StatisticDialogPresenter::StatisticDialogPresenter()
{

}



std::optional<DentalStatistic> StatisticDialogPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(*this);

	return m_statistic;
}

void StatisticDialogPresenter::setProcedureType(int type)
{

	view->setDiagnosisFilterList(DbStat::getDiagnosis(type));
	view->setNameFilterList(DbStat::getProcedureNames(type));

	auto procedureType = static_cast<ProcedureType>(type);

	bool hideToothFilter = 
			procedureType == ProcedureType::general ||
			procedureType == ProcedureType::bridge 	||
			procedureType == ProcedureType::fibersplint;

	view->hideToothFilter(hideToothFilter);
}

void StatisticDialogPresenter::setView(IStatisticDialogView* view)
{
	this->view = view;

	if (m_statistic) {
		view->setStatistic(m_statistic.value());
		return;
	}


	view->setDiagnosisFilterList(DbStat::getDiagnosis(0));
	view->setNameFilterList(DbStat::getProcedureNames(0));
	view->hideToothFilter(true);
}

void StatisticDialogPresenter::okPressed()
{
	m_statistic = view->getStatistic();

	view->closeDialog();
}
