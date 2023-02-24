#include "ProcedureCreator.h"
#include "Model/Dental/KSMP.h"
#include "View/ModalDialogBuilder.h"


ProcedureCreator::ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth)
	: m_selectedTeeth(selectedTeeth)
{


}


std::string ProcedureCreator::restorationDiagnosis(const Tooth& tooth)
{
	bool secondaryCaries = false;

	for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
	{
		if (tooth.caries.exists(i) && tooth.obturation.exists(i))
		{
			secondaryCaries = true;
		}
	}

	std::array<bool, 7> existing
	{
			tooth.fracture.exists(),
			secondaryCaries,
			tooth.caries.exists(),
			tooth.endo.exists(),
			tooth.pulpitis.exists() && tooth.lesion.exists(),
			tooth.root.exists(),
			tooth.obturation.exists()
	};

	std::array<std::string, 7> diagnosis
	{
		"Фрактура",
		"Вторичен кариес",
		"Кариес",
		"Status post devital.",
		"Преендодонтско изграждане",
		"Изграждане на корен за корона",
		"Дефектна обтурация"
	};

	for (int i = 0; i < 7; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}

std::string ProcedureCreator::extractionDiagnosis(const Tooth& tooth)
{
	std::array<bool, 9> existing
	{
		tooth.hyperdontic.exists(),
		tooth.implant.exists(),
		tooth.lesion.exists(),
		tooth.temporary.exists(),
		tooth.root.exists(),
		tooth.periodontitis.exists(),
		tooth.mobility.exists(),
		tooth.fracture.exists(),
		tooth.pulpitis.exists()
	};

	std::array<std::string, 9> diagnosis
	{
		"Свръхброен зъб",
		"Периимплантит",
		"Periodontitis chronica granulomatosa localisata",
		"Разклатен млечен зъб",
		"Radix relicva",
		"Пародонтозен зъб",
		"Подвижен зъб",
		"Фрактура",
		"Pulpitis chronica ulcerosa"
	};

	for (int i = 0; i < 9; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string{};
}

std::string ProcedureCreator::endodonticDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.pulpitis.exists(),
		tooth.lesion.exists(),
		tooth.endo.exists(),
		tooth.fracture.exists()
	};

	std::array<std::string, 4> diagnosis
	{
		"Pulpitis chronica ulcerosa",
		"Periodontitis chronica granulomatosa localisata",
		"Инфектиран коренов пълнеж",
		"Фрактура"
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return std::string();
}

std::string ProcedureCreator::crownDiagnosis(const Tooth& tooth)
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
		"Фрактура",
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

std::string ProcedureCreator::implantDiagnosis(const Tooth& tooth)
{
		if (tooth.extraction) return "Andontia partialis";

		return std::string{};
}

std::array<bool, 6> ProcedureCreator::autoSurfaces(const Tooth& tooth)
{
	std::array<bool, 6> surfaces{ 0,0,0,0,0,0 };

	for (int i = 0; i < 6; i++)
	{
		surfaces[i] = tooth.caries.exists(i);
	}

	if (tooth.root.exists())
	{
		surfaces = { 1, 1, 1, 1, 1, 1 };
	}

	if (tooth.endo.exists())
	{
		if (ToothUtils::getToothType(tooth.index) == ToothType::Frontal)
		{
			surfaces[Surface::Lingual] = true;
		}
		else surfaces[Surface::Occlusal] = true;
	}

	if (tooth.fracture.exists())
	{
		if (ToothUtils::getToothType(tooth.index) == ToothType::Frontal)
		{
			surfaces[Surface::Occlusal] = true;
		}
		else surfaces[Surface::Lingual] = true;
	}
	else if (tooth.obturation.exists() && !tooth.caries.exists())
	{
		for (int i = 0; i < 6; i++)
		{
			if (tooth.obturation.exists(i))
				surfaces[i] = true;
		}
	}

	return surfaces;
}

ConstructionRange ProcedureCreator::getBridgeRange(const std::vector<const Tooth*> selectedTeeth)
{
	int begin;
	int end;

	if (!selectedTeeth.size())
	{
		begin = 0;
		end = 1;
		return { begin, end };
	}

	begin = 0;
	end = 0;

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
		return { begin, end };
	}

	begin = selectedTeeth[0]->index; //if multiple teeth are selected, the range is calculated to be valid
	end = selectedTeeth.back()->index; //doesn't matter if the first and last teeth are on different jaws

	if (begin < 16 && end > 15)
	{
		end = 15;
	}

	return { begin, end };
}

std::string ProcedureCreator::bridgeOrFiberDiagnosis(const std::vector<const Tooth*> selectedTeeth, ConstructionRange& range)
{
	for (int i = range.tooth_begin; i <= range.tooth_end; i++)
	{
		auto& tooth = selectedTeeth.at(i);

		if (tooth->extraction) return "Andontia partialis";
	}

	return "Стабилизация с блок корони";
}

void ProcedureCreator::setView(ICommonFields* view)
{
	this->view = view;

	if (m_selectedTeeth.size()) {
		view->surfaceSelector()->setData(ProcedureObtData{ autoSurfaces(*m_selectedTeeth[0]), false });
	}

	auto [begin, end] = getBridgeRange(m_selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);


}

void ProcedureCreator::setProcedureCode(const ProcedureCode& m)
{
	view->setCurrentPresenter(this);

	m_code = m;

	//If the template doesn't have default diagnosis, 
	//each presenter class auto-generates one if possible.
	//The implementation-defined diagnosis can be generated either
	//in the constructor of the subclass, or in the overriden getProcedure func:

	view->diagnosisEdit()->set_Text(m_diagnosis);

	view->diagnosisEdit()->setInputValidator(&notEmpty_validator);

	view->setNhifLayout(m.nhifCode() != 0);

	

	switch (m.type())
	{
		case ProcedureType::obturation:
			if (m_selectedTeeth.empty()) {
				view->setErrorMsg("Изберете поне един зъб!");
				break;
			}
			view->setLayout(ICommonFields::Restoration); 
			view->surfaceSelector()->setInputValidator(&surface_validator);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
			view->setLayout(ICommonFields::Range);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(&range_validator);
			break;
		case ProcedureType::removebridgeOrSplint:
			view->setLayout(ICommonFields::Range);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		case ProcedureType::general:
			view->setLayout(ICommonFields::General);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		default:
			view->setLayout(ICommonFields::ToothSpecific);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			if (m_selectedTeeth.empty()) {
				view->setErrorMsg("Изберете поне един зъб!");
				break;
			}
			break;
	}

}

bool ProcedureCreator::isValid()
{

	std::vector<AbstractUIElement*> validatable{
		view->dateEdit(),
		view->diagnosisEdit(),
		view->surfaceSelector(),
		view->rangeWidget()
	};


	for (AbstractUIElement* e : validatable)
	{
		e->validateInput();
		if (!e->isValid())
		{
			e->setFocus();
			return false;
		}
	}

	return true;
}

std::vector<Procedure> ProcedureCreator::getProcedures()
{
	Procedure procedure;

	std::vector<Procedure> result;

	procedure.code = m_code;
	procedure.financingSource = view->getFinancingSource();
	procedure.LPK = User::doctor().LPK;
	procedure.date = view->dateEdit()->getDate();
	procedure.diagnosis = view->diagnosisEdit()->getText();
	procedure.notes = view->getNotes();
	procedure.hyperdontic = view->onHyperdontic();
	//	result.price = common_view->priceEdit()->get_Value();
	procedure.tooth = -1;

	switch (procedure.code.type())
	{
	case ProcedureType::general:
		procedure.result = NoData{};
		result.push_back(procedure);
		break;
	case ProcedureType::obturation:
		procedure.result = view->surfaceSelector()->getData();
		for (auto t : m_selectedTeeth)
		{
			result.push_back(procedure);
			result.back().tooth = t->index;
		}
		break;
	case ProcedureType::bridge:
	case ProcedureType::fibersplint:
	case ProcedureType::removebridgeOrSplint:
	{
		auto [begin, end] = view->rangeWidget()->getRange();
		procedure.result = ConstructionRange{ begin, end };
		result.push_back(procedure);
	}
		break;
	default:
	{
		procedure.result = NoData{};
		for (auto t : m_selectedTeeth)
		{
			result.push_back(procedure);
			result.back().tooth = t->index;
		}
	}
		break;
		
	}


	return result;
}



