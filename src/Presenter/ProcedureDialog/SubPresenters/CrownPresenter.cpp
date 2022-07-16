#include "CrownPresenter.h"

#include "Model/Tooth/ToothContainer.h"
#include "Model/KSMP.h"
CrownPresenter::CrownPresenter(
	const std::vector<Tooth*>& selectedTeeth,
	const ToothContainer& teeth
)
	:
	AbstractSubPresenter(ProcedureType::crown),
    teeth(teeth),
	selectedTeeth(selectedTeeth),
	view(nullptr),
	m_bridgeSelected(false),
	m_bridgePrice(0),
	ksmp_other(KSMP::getByType(ProcedureType::bridge).at(1)->code)
{

	if (selectedTeeth.size() == 1)
	{
		m_diagnosis = getDiagnosis(*selectedTeeth.front());
	}
}

void CrownPresenter::setProcedureTemplate(const ProcedureTemplate& m)
{
	//remembering the bridge ksmp, otherwise it will be replaced
	if (m_bridgeSelected) {
		std::swap(m_ksmp.code, ksmp_other);
	}

	AbstractSubPresenter::setProcedureTemplate(m);

	auto crownData = view->getData();
	crownData.material = m.material;

	auto [begin, end] = view->rangeWidget()->getRange();
	int length = end - begin + 1;
	m_bridgePrice = m_price * length;

	if (!m_bridgeSelected) {
		view->setData(crownData);
		return;
	}
	else
	{
		view->setData(
			ProcedureBridgeData{
				begin, end,
				crownData

			});
	}

	if (!range_validator.validateInput(begin, end))
		m_bridgePrice = 0;

	selectAsBridge(true);

}

void CrownPresenter::selectAsBridge(bool checked)
{
	std::swap(m_ksmp.code, ksmp_other);
	common_view->setKSMPCode(m_ksmp.code);

	if (checked)
	{
		m_bridgeSelected = true;
		common_view->diagnosisEdit()->set_Text(m_bridgeDiagnosis);
		common_view->procedureNameEdit()->set_Text(m_name + m_bridgeRangeString);
		common_view->priceEdit()->set_Value(m_bridgePrice);
		m_type = ProcedureType::bridge;



		return;
	}

	m_bridgeSelected = false;
	common_view->diagnosisEdit()->set_Text(m_diagnosis);
	common_view->procedureNameEdit()->set_Text(m_name);
	common_view->priceEdit()->set_Value(m_price);
	m_type = ProcedureType::crown;

}

std::vector<Procedure> CrownPresenter::getProcedures()
{
	std::vector<Procedure> procedures;

	if (!m_bridgeSelected)
	{
		procedures.reserve(selectedTeeth.size());

		for (auto& tooth : selectedTeeth) {
			procedures.push_back(AbstractSubPresenter::getProcedureCommonFields());
			procedures.back().result = view->getData();
			procedures.back().tooth = tooth->index;
			procedures.back().temp = tooth->temporary.exists();
		}

		return procedures;
	}

	
	auto procedure = AbstractSubPresenter::getProcedureCommonFields();
	procedure.type = ProcedureType::bridge;

	auto [begin, end] = view->rangeWidget()->getRange();
	auto data = view->getData();

	procedure.result = ProcedureBridgeData{
		begin, end, data
	};

	return {procedure};
}

bool CrownPresenter::isValid()
{
	if (!AbstractSubPresenter::isValid()) return false;

	if (!selectedTeeth.size()) return false;

	if (m_bridgeSelected)
	{
		auto rW = view->rangeWidget();
		
		rW->validateInput();
		if (!rW->isValid())
		{
			rW->setFocus();
			return false;
		}
	}

	return true;
}


void CrownPresenter::setView(ICrownView* view)
{
	this->view = view;
	auto [begin, end] = getInitialBridgeRange(selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);
	m_bridgeDiagnosis = getBridgeDiagnosis(begin, end, teeth);
	m_bridgeRangeString = getBridgeRangeName(begin, end, teeth);
	

	view->rangeWidget()->setInputValidator(&range_validator);

	if (selectedTeeth.empty()) //only bridge can be generated
	{	
		view->lockBridgeCheckbox();
		m_bridgeSelected = true;
	}
		

}


void CrownPresenter::rangeChanged(int begin, int end)
{
	m_bridgeDiagnosis = getBridgeDiagnosis(begin, end, teeth);
	m_bridgeRangeString = getBridgeRangeName(begin, end, teeth);

	int length = end - begin + 1;
	m_bridgePrice = m_price * length;

	common_view->diagnosisEdit()->set_Text(m_bridgeDiagnosis);
	common_view->procedureNameEdit()->set_Text(m_name + m_bridgeRangeString);
	common_view->priceEdit()->set_Value(m_bridgePrice);
}

std::string CrownPresenter::getDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.endo.exists(),
		tooth.fracture.exists(),
		tooth.obturation.exists(),
		tooth.implant.exists()
	};

	std::array<std::string, 4> diagnosis
	{
		"Status post devitalisationem",
		tooth.fracture.data.getDiagnosisName(),
		"Екстензивна ресторация",
		"Протезиране върху имплант"
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}

std::tuple<int, int> CrownPresenter::getInitialBridgeRange(const std::vector<Tooth*>& selectedTeeth)
{
	if (!selectedTeeth.size())
	{
		return std::tuple<int, int>(0, 1);
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

std::string CrownPresenter::getBridgeDiagnosis(int begin, int end, const ToothContainer& teeth)
{
	std::vector<const Tooth*> teethRange;

	for (int i = begin; i < end + 1; i++)
	{
		teethRange.push_back(&teeth.at(i));
	}

	std::vector<int> missingTeethIdx;

	for (const Tooth* tooth : teethRange)
	{
		if (tooth->extraction.exists())
		{
			missingTeethIdx.emplace_back(ToothUtils::getToothNumber(tooth->index, false));
		}
	}


	if (missingTeethIdx.size() == teethRange.size())
	{
		return "Всички зъби са извадени, wtf?"; //all selected teeth are extracted, wtf user?
	}

	if (!missingTeethIdx.size())
	{
		return "Стабилизация с блок корони";
	}

	std::string bridgeDiagnosis;
	bridgeDiagnosis.reserve(5 * missingTeethIdx.size() + 19);
	bridgeDiagnosis.append("Adontia partialis ");

	for (int i = 0; i < missingTeethIdx.size(); i++)
	{

		bridgeDiagnosis.append(std::to_string(missingTeethIdx.at(i)));

		if (i != missingTeethIdx.size() - 1)
		{
			bridgeDiagnosis.append(", ");
		}
	}

	return bridgeDiagnosis;
}

std::string CrownPresenter::getBridgeRangeName(int begin, int end, const ToothContainer& teeth)
{
	return std::string
	{
		" (конструкция " +
		std::to_string(ToothUtils::getToothNumber(begin, teeth[begin].temporary.exists())) +
		" - " +
		std::to_string(ToothUtils::getToothNumber(end, teeth[end].temporary.exists())) + ")"
	};
}