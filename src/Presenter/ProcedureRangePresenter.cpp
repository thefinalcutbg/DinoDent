#include "ProcedureRangePresenter.h"

ProcedureRangePresenter::ProcedureRangePresenter(const std::vector<Tooth*>& selectedTeeth, ProcedureType type) :
	AbstractSubPresenter(type), selectedTeeth(selectedTeeth)
{
	if (!selectedTeeth.size())
	{
		begin = 0;
		end = 0;
		return;
	}

	if (selectedTeeth.size() == 1) //if only 1 tooth is selected, the bridge length is 2
	{
		begin = selectedTeeth[0]->index;

		end = begin;

		return;
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return;
}

void ProcedureRangePresenter::setView(AbstractRangeEdit* view)
{
	this->view = view;
	view->setBridgeRange(begin, end);
}

std::vector<Procedure> ProcedureRangePresenter::getProcedures()
{
	auto procedure = AbstractSubPresenter::getProcedureCommonFields();

	auto [begin, end] = view->getRange();

	procedure.result = ProcedureRangeRemoveData{
		begin,
		end
	};

	return { procedure };
}
