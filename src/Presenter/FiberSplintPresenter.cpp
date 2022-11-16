#include "FiberSplintPresenter.h"
#include "Model/Dental/ToothContainer.h"

FiberSplintPresenter::FiberSplintPresenter(const std::vector<Tooth*>& selectedTeeth,
	const ToothContainer& teeth)
	:
	AbstractSubPresenter(ProcedureType::fibersplint),
	selectedTeeth(selectedTeeth),
	teeth(teeth),
	view(nullptr)
{

}

void FiberSplintPresenter::setView(IFiberSplintView* view)
{
	this->view = view;
	auto [begin, end] = getInitialSplintRange(selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);
	view->rangeWidget()->setInputValidator(&range_validator);

//	m_diagnosis = getDiagnosis();
//	splintRangeName = getSplintRangeName(begin, end, teeth);
}

void FiberSplintPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	AbstractSubPresenter::setProcedureTemplate(m);

	auto data = view->getData();

	//data.material = m.material;
	auto [begin, end] = view->rangeWidget()->getRange();

	view->setData(ProcedureFiberData{ begin, end, data });

	rangeChanged(begin, end);

}

void FiberSplintPresenter::rangeChanged(int begin, int end)
{
	if (!range_validator.validateInput(begin, end)) return;

	m_diagnosis = getDiagnosis();
	splintRangeName = getSplintRangeName(begin, end, teeth);

	common_view->diagnosisEdit()->set_Text(m_diagnosis);
	common_view->procedureNameEdit()->set_Text(m_name + splintRangeName);
//	common_view->priceEdit()->set_Value((end - begin + 1) * m_price);
}


std::string FiberSplintPresenter::getSplintRangeName(int begin, int end, const ToothContainer& teeth)
{

	return std::string
	{
		" (" +
		std::to_string(ToothUtils::getToothNumber(begin, teeth[begin].temporary.exists())) +
		" - " +
		std::to_string(ToothUtils::getToothNumber(end, teeth[end].temporary.exists())) + ")"
	};
}

std::tuple<int, int> FiberSplintPresenter::getInitialSplintRange(const std::vector<Tooth*>& selectedTeeth)
{
	

	if (!selectedTeeth.size())
	{
		return std::tuple<int, int>(21, 26);
	}

	int begin = 0;
	int end = 0;

	if (selectedTeeth.size() == 1) //if only 1 tooth is selected, the bridge length is 2
	{
		begin = selectedTeeth[0]->index;

		if (begin != 15 && begin != 31)
		{
			end = begin + 1;
		}
		else
		{
			end = begin - 1;
			std::swap(end, begin);
		}
		return std::tuple<int, int>{begin, end};
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return std::tuple<int, int>(begin, end);
	
}

std::string FiberSplintPresenter::getDiagnosis()
{
	auto [begin, end] = view->rangeWidget()->getRange();

	std::vector<int> missingTeeth;

	for (int i = begin; i <= end; i++)
	{
		const Tooth& tooth = teeth[i];
		if (tooth.extraction.exists() || tooth.impacted.exists())
			missingTeeth.push_back(tooth.index);
	}

	if (!missingTeeth.size())
		return "Пародонтално компрометирани зъби";

	std::string diagnosis{ "Adontia " };

	for (int i = 0; i < missingTeeth.size(); i++)
	{

		diagnosis += ToothUtils::getNomenclature(missingTeeth[i]);

		if (i != missingTeeth.size() - 1)
			diagnosis += ", ";
	}

	return diagnosis;
}



bool FiberSplintPresenter::isValid()
{
	if (!AbstractSubPresenter::isValid()) return false;

	auto rW = view->rangeWidget();

	rW->validateInput();
	if (!rW->isValid())
	{
		rW->setFocus();
		return false;
	}

	return true;
}

std::vector<Procedure> FiberSplintPresenter::getProcedures()
{
	auto procedure = AbstractSubPresenter::getProcedureCommonFields();
	procedure.type = ProcedureType::fibersplint;

	auto [begin, end] = view->rangeWidget()->getRange();

	procedure.result = ProcedureFiberData{

		begin,
		end,
		view->getData()

	};

	return { procedure };
}
