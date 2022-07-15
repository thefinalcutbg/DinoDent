#include "FiberSplintPresenter.h"
#include "Model/Tooth/ToothContainer.h"

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
		return u8"Пародонтално компрометирани зъби";

	std::string diagnosis{ "Adontia " };

	for (int i = 0; i < missingTeeth.size(); i++)
	{

		diagnosis += ToothUtils::getNomenclature(missingTeeth[i]);

		if (i != missingTeeth.size() - 1)
			diagnosis += ", ";
	}

	return diagnosis;
}


FiberSplintPresenter::FiberSplintPresenter(const std::vector<Tooth*>& selectedTeeth, 
										   const ToothContainer& teeth)
	:
	AbstractSubPresenter(ProcedureType::fibersplint),
	selectedTeeth(selectedTeeth),
	teeth(teeth),
	view(nullptr),
	m_price(0)
{

}

void FiberSplintPresenter::setView(IFiberSplintView* view)
{
	this->view = view;
	auto [begin, end] = getInitialSplintRange(selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);
	m_diagnosis = getDiagnosis();
	splintRangeName = getSplintRangeName(begin, end, teeth);


	view->rangeWidget()->setInputValidator(&range_validator);
}

void FiberSplintPresenter::rangeChanged(int begin, int end)
{
	m_diagnosis = getDiagnosis();
	splintRangeName = getSplintRangeName(begin, end, teeth);

	m_price = (end - begin + 1) * m_price;

	common_view->diagnosisEdit()->set_Text(m_diagnosis);
	common_view->procedureNameEdit()->set_Text(m_name + splintRangeName);
	common_view->priceEdit()->set_Value(m_price);
}


void FiberSplintPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{

	common_view->set_hidden(false);
	view->set_hidden(false);

	AbstractSubPresenter::setProcedureTemplate(m);
	common_view->procedureNameEdit()->set_Text(m.name + splintRangeName);

	auto [begin, end] = view->rangeWidget()->getRange();
	int length = end - begin + 1;
	m_price = m_price * length;

	auto data = view->getData();
	data.material = m.material;
	view->setData(ProcedureFiberData{begin, end, data });

	if (!range_validator.validateInput(begin, end))
		m_price = 0;

	common_view->priceEdit()->set_Value(m_price);

	view->setData(ProcedureFiberData{begin, end, data});

}

bool FiberSplintPresenter::isValid()
{
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
