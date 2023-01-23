#include "EditorPresenter.h"
#include "Model/Dental/KSMP.h"


EditorPresenter::EditorPresenter(const Procedure& p) :
	AbstractSubPresenter(p.type)
{
	m_code = p.code;
	//m_price = p.price;
	m_nzok = p.isNhif();
	m_name = p.name;
	m_temp = p.temp;
	m_tooth = p.tooth;
	m_date = p.date;
	m_diagnosis = p.diagnosis;

	p.ksmp.empty() ?
		m_ksmp = KSMP::getByType(p.type).at(0)->code
		:
		m_ksmp = p.ksmp;

	

}

void EditorPresenter::setAdditionalTemplateParameters()
{
	view->procedureNameEdit()->set_Text(m_name);
	//	common_view->priceEdit()->set_Value(m_price);
	view->diagnosisEdit()->set_Text(m_diagnosis);
	view->diagnosisEdit()->setInputValidator(&notEmpty_validator);
	view->procedureNameEdit()->setInputValidator(&notEmpty_validator);
	view->dateEdit()->set_Date(m_date);
	
	view->setKSMPCode(m_ksmp);

	switch (m_type)
	{
	case ProcedureType::obturation:
		view->setLayout(ICommonFields::Restoration, true);
		view->surfaceSelector()->setInputValidator(&surface_validator);
		break;
	case ProcedureType::removebridgeOrSplint:
		view->setLayout(ICommonFields::Range);
		view->rangeWidget()->setInputValidator(nullptr);
		break;
	case ProcedureType::bridge:
	case ProcedureType::fibersplint:
		view->setLayout(ICommonFields::Range);
		view->rangeWidget()->setInputValidator(&range_validator);
		break;
	default:
		view->setLayout(ICommonFields::General, m_type != ProcedureType::general);
	}
}

bool EditorPresenter::additionalValidation()
{
	
	return 
		view->rangeWidget()->validateInput() && 
		view->surfaceSelector()->validateInput()
	;
}

std::vector<Procedure> EditorPresenter::getProcedures()
{
	std::vector<Procedure> result{ getProcedureCommonFields() };

	auto& p = result.back();

	p.tooth = m_tooth;
	p.temp = m_temp;
	
	return result;
}
