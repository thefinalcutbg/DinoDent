#include "ToothProcedurePresenter.h"


ToothProcedurePresenter::ToothProcedurePresenter(const std::vector<Tooth*>& selectedTeeth)
	:
	AbstractSubPresenter(ProcedureType::any),
	selectedTeeth(selectedTeeth)
{
	
}

void ToothProcedurePresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	bool noTeethSelected = !selectedTeeth.size();
	common_view->set_hidden(noTeethSelected);
	if (noTeethSelected) return;

	AbstractSubPresenter::setProcedureTemplate(m);
}

std::vector<Procedure> ToothProcedurePresenter::getProcedures()
{
	std::vector<Procedure> procedures;
	procedures.reserve(selectedTeeth.size());

	for (auto& tooth : selectedTeeth)
	{
		procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
		procedures.back().tooth = tooth->index;
		procedures.back().temp = tooth->temporary.exists();
	}

	return procedures;

}
