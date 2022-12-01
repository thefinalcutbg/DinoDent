#include "ReferralPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbReferral.h"
#include "Model/Dental/AmbList.h"

ReferralPresenter::ReferralPresenter(const AmbList& sheet, ReferralType t) : m_type(t)
{
	m_result.emplace(t);

	//determening the sheet number
	m_result->number = DbReferral::getRefNumber(t, sheet.getDate().year);

	for (auto& r : sheet.referrals)
	{
		if (r.type == t) {
			m_result->number = std::max(m_result->number, r.number);
		}

	}

	if (sheet.referrals.size()) {
		m_result->number++;
	}

	//determening the date
	auto sheetDate = sheet.getDate();

	if (sheetDate.isFromPreviousMonths(Date::currentDate())) {
		m_result->date = sheetDate;
	}
	 
	
}

ReferralPresenter::ReferralPresenter(const Referral& r) : 
	m_type(r.type), m_result(r), ref_rowid{ r.rowid }
{
	
}

void ReferralPresenter::okPressed()
{
	auto common = view->getCommon();

	if (DbReferral::refNumberExists(m_type, common.date.year, ref_rowid) &&
		!ModalDialogBuilder::askDialog(
				"Направление с такъв номер вече съществува.\n"
				"Сигурни ли сте, че искате да дублирате номерацията?"
			)
	){ return; } 

	if (MKB::getNameFromMKBCode(common.mkb).empty()) {
		view->setErrorLabel("Моля, изберете диагноза по МКБ!");
		return;
	}

	m_result.emplace(m_type);

	m_result->rowid = ref_rowid;
	m_result->date = common.date;
	m_result->diagnosis.main = common.mkb;
	m_result->number = common.number;
	m_result->reason = common.reason_idx;

	switch (m_type) {
		case ReferralType::MDD4:
			m_result->data = view->MDD4data(); break;
		case ReferralType::Form3:
			m_result->data = R3Data(); break;
		case ReferralType::Form3A:
			m_result->data = R3AData(); break;
	}

	view->closeDialog();
}

void ReferralPresenter::setView(IReferralDialog* view)
{
	this->view = view;

	switch (m_type)
	{
		case ReferralType::MDD4:
			view->setTitle("Направление за рентгенография (МДД бл.4)"); break;
		case ReferralType::Form3:
			view->setTitle("Направление за психиатър (бл.3)"); break;
		case ReferralType::Form3A:
			view->setTitle("Направление за анестезиолог (бл.3А)"); break;
	}

	view->setReferral(m_result.value());

	m_result.reset();

}

std::optional<Referral> ReferralPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return m_result;
}