#include "EditorPresenter.h"
#include "Model/KSMP.h"


EditorPresenter::EditorPresenter(const Procedure& p) :
	AbstractSubPresenter(p.type)
{
	m_code = p.code;
	m_price = p.price;
	m_nzok = p.nzok;
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

void EditorPresenter::setCommonFieldsView(ICommonFields* view)
{
	this->common_view = view;
	common_view->setCurrentPresenter(this);

	common_view->procedureNameEdit()->disable(m_nzok);

	common_view->procedureNameEdit()->set_Text(m_name);
	common_view->priceEdit()->set_Value(m_price);
	common_view->diagnosisEdit()->set_Text(m_diagnosis);
	common_view->diagnosisEdit()->setInputValidator(&notEmpty_validator);
	common_view->procedureNameEdit()->setInputValidator(&notEmpty_validator);
	common_view->dateEdit()->set_Date(m_date);
	common_view->enableKSMP(m_nzok == false);

	common_view->setKSMPCode(m_ksmp);

}

std::vector<Procedure> EditorPresenter::getProcedures()
{
	std::vector<Procedure> result{ getProcedureCommonFields() };

	auto& p = result.back();

	p.tooth = m_tooth;
	p.temp = m_temp;
	
	return result;
}
