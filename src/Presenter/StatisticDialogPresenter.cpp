#include "StatisticDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbStatistics.h"
#include "Model/Dental/Procedure.h"

StatisticDialogPresenter::StatisticDialogPresenter()
{

}

void StatisticDialogPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(*this);
}

void StatisticDialogPresenter::setProcedureType(int type)
{

	auto procedureType = static_cast<ProcedureType>(type);

	bool hideToothFilter = ProcedureCode::getScope(procedureType) != ProcedureScope::SingleTooth;

	view->hideToothFilter(hideToothFilter);

	view->setProcedureFilter(ProcedureCode::getByType(procedureType));
}

void StatisticDialogPresenter::setView(IStatisticDialogView* view)
{
	this->view = view;

	view->setProcedureFilter(ProcedureCode::getByType(ProcedureType::General));
	view->hideToothFilter(true);
}

void StatisticDialogPresenter::parameterChanged()
{
	std::string totalCount = "Намерени резултати: ";
	view->setResult(DbStat::count(view->getStatistic()));
}
