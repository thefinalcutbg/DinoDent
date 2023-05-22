#include "ProcedureCreator.h"
#include "Model/Dental/KSMP.h"
#include "View/ModalDialogBuilder.h"


ProcedureCreator::ProcedureCreator(const std::vector<const Tooth*>& selectedTeeth)
	: m_selectedTeeth(selectedTeeth)
{


}


int ProcedureCreator::restorationDiagnosis(const Tooth& tooth)
{

	bool secondaryCaries = false;

	for (int i = 0; i < 6; i++)		//checking if somewhere obturation is present also, returning secondary caries
	{
		if (tooth.caries.exists(i) && tooth.obturation.exists(i))
		{
			secondaryCaries = true;
		}
	}

	std::array<bool, 6> existing
	{
			tooth.fracture.exists(),
			tooth.caries.exists(),
			tooth.endo.exists(),
			tooth.pulpitis.exists(),
			tooth.lesion.exists(),
			tooth.root.exists(),
	};

	std::array<int, 7> diagnosis
	{
		4,
		1,
		8,
		2,
		3,
		4, //should be root
	};

	for (int i = 0; i < 6; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::extractionDiagnosis(const Tooth& tooth)
{
	std::array<bool, 7> existing
	{
		tooth.lesion.exists(),
		tooth.temporary.exists(),
		tooth.root.exists(),
		tooth.periodontitis.exists(),
		tooth.mobility.exists(),
		tooth.fracture.exists(),
		tooth.pulpitis.exists()
	};

	std::array<int, 7> diagnosis
	{
		3,
		7,	//should be deciduous
		4,	//should be root
		7,
		7,
		4,
		2
	};

	for (int i = 0; i < 7; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::endodonticDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.pulpitis.exists(),
		tooth.lesion.exists(),
		tooth.endo.exists(),
		tooth.fracture.exists()
	};

	std::array<int, 4> diagnosis
	{
		2,
		3,
		8,
		4
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 9;
}

int ProcedureCreator::crownDiagnosis(const Tooth& tooth)
{
	std::array<bool, 4> existing
	{
		tooth.endo.exists(),
		tooth.fracture.exists(),
		tooth.obturation.exists(),
		tooth.implant.exists()
	};

	std::array<int, 4> diagnosis
	{
		8,
		4,
		1, //should be extensive restoration
		5
	};

	for (int i = 0; i < 4; i++)
	{
		if (existing[i]) {
			return diagnosis[i];
		}
	}

	return 0;
}

int ProcedureCreator::implantDiagnosis(const Tooth& tooth)
{
	if (tooth.extraction) return 5;

		return 0;
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

	if (begin < 16 && end > 15) { end = 15; }

	return { begin, end };
}

std::string ProcedureCreator::bridgeOrFiberDiagnosis(const std::vector<const Tooth*> selectedTeeth, const ConstructionRange& range)
{
	for (int i = range.tooth_begin; i <= range.tooth_end; i++)
	{
		auto& tooth = selectedTeeth.at(i);

		if (tooth->extraction) return "Andontia partialis";
	}

	return "Стабилизация с блок корони";
}

void ProcedureCreator::setView(IProcedureInput* view)
{
	this->view = view;

	if (m_selectedTeeth.size()) {

		auto t = m_selectedTeeth[0];

		view->surfaceSelector()->setData(RestorationData{ autoSurfaces(*t), false });
		diag_map[ProcedureType::obturation] = restorationDiagnosis(*t);
		diag_map[ProcedureType::extraction] = extractionDiagnosis(*t);
		diag_map[ProcedureType::endo] = endodonticDiagnosis(*t);
		diag_map[ProcedureType::crown] = crownDiagnosis(*t);
		diag_map[ProcedureType::implant] = implantDiagnosis(*t);
	}

	diag_map[ProcedureType::bridge] = 5;
	diag_map[ProcedureType::fibersplint] = 7;
	diag_map[ProcedureType::denture] = 6;
	diag_map[ProcedureType::depuratio] = 10;

	auto [begin, end] = getBridgeRange(m_selectedTeeth);

	view->rangeWidget()->setBridgeRange(begin, end);

	view->setCurrentPresenter(this);



	//diag_map[ProcedureType::bridge] = bridgeOrFiberDiagnosis(m_selectedTeeth, ConstructionRange{ begin, end });
	//diag_map[ProcedureType::fibersplint] = bridgeOrFiberDiagnosis(m_selectedTeeth, ConstructionRange{ begin, end });

}

void ProcedureCreator::diagnosisChanged(int idx)
{
	diag_map[m_code.type()] = idx;

}

void ProcedureCreator::setProcedureCode(const ProcedureCode& m, bool nhif)
{
	view->setCurrentPresenter(this);

	m_code = m;

	auto diagIdx = diag_map[m.type()];

	view->diagnosisCombo()->setIndex(diag_map[m.type()]);

	view->diagnosisEdit()->setInputValidator(diagIdx ? nullptr : &notEmpty_validator);

	view->setNhifLayout(nhif);

	if (!diagIdx && m.oldCode() == 101) {
		view->diagnosisEdit()->set_Text("Преглед");
	}
	else
	{
		view->diagnosisEdit()->set_Text("");
	}
	
	view->diagnosisEdit()->validateInput();

	switch (m.type())
	{
		case ProcedureType::obturation:
			if (m_selectedTeeth.empty()) {
				view->setErrorMsg("Изберете поне един зъб!");
				break;
			}
			view->setLayout(IProcedureInput::Restoration); 
			view->surfaceSelector()->setInputValidator(&surface_validator);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		case ProcedureType::bridge:
		case ProcedureType::fibersplint:
		case ProcedureType::denture:
			view->setLayout(IProcedureInput::Range);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(&range_validator);
			break;
			/*
		case ProcedureType::removebridgeOrSplint:
			view->setLayout(IProcedureInput::Range);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
			*/
		case ProcedureType::depuratio:
		case ProcedureType::general:
		case ProcedureType::full_exam:
			view->setLayout(IProcedureInput::General);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		case ProcedureType::anesthesia:
			view->setLayout(IProcedureInput::Anesthesia);
			view->surfaceSelector()->setInputValidator(nullptr);
			view->rangeWidget()->setInputValidator(nullptr);
			break;
		default:
			view->setLayout(IProcedureInput::ToothSpecific);
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
		view->surfaceSelector(),
		view->rangeWidget(),
		view->diagnosisEdit()
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
	procedure.diagnosis = view->diagnosisCombo()->getIndex();
	procedure.diagnosis.description = view->diagnosisEdit()->getText();
	procedure.notes = view->getNotes();
	procedure.tooth_idx.supernumeral = view->onHyperdontic();


	switch (procedure.code.type())
	{
	case ProcedureType::depuratio:
	case ProcedureType::general:
	case ProcedureType::full_exam:
		result.push_back(procedure);
		break;
	case ProcedureType::obturation:
		procedure.result = view->surfaceSelector()->getData();
		for (auto t : m_selectedTeeth)
		{
			result.push_back(procedure);
			result.back().tooth_idx.index = t->index;
		}
		break;
	case ProcedureType::bridge:
	case ProcedureType::fibersplint:
	case ProcedureType::denture:
	//case ProcedureType::removebridgeOrSplint:
	{
		auto [begin, end] = view->rangeWidget()->getRange();
		procedure.result = ConstructionRange{ begin, end };
		result.push_back(procedure);
	}
		break;
	case ProcedureType::anesthesia:
	{
		procedure.result = AnesthesiaMinutes{ view->minutes() };
		result.push_back(procedure);
		break;
	}
	default:
	{
		procedure.result = std::monostate{};

		for (auto t : m_selectedTeeth)
		{
			result.push_back(procedure);
			result.back().tooth_idx.index = t->index;
		}
	}
		break;
		
	}

	return result;
}



