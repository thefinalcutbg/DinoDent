#include "ReferralPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbReferral.h"
#include "Model/Dental/AmbList.h"
#include "Model/FreeFunctions.h"

ReferralPresenter::ReferralPresenter(const AmbList& sheet, ReferralType t) : 
	m_type(t),
	lrn{ FreeFn::getUuid() }
{
	m_result.emplace(t);

	if (!m_result->isNrnType()) {

		//determening the sheet number
		m_result->number = DbReferral::getRefNumber(t, sheet.getDate().year);

		bool hasSuchReferral{ false };

		for (auto& r : sheet.referrals)
		{
			if (r.type == t) {
				hasSuchReferral = true;
				m_result->number = std::max(m_result->number, r.number);
			}

		}

		if (hasSuchReferral) {
			m_result->number++;
		}
	}

	//determening the date
	auto sheetDate = sheet.getDate();

	if (sheetDate.isFromPreviousMonths(Date::currentDate())) {
		m_result->date = sheetDate;
	}
	
	
}

void ReferralPresenter::setView(IReferralDialog* view)
{
	this->view = view;

	view->setTitle(m_result->getTypeAsString());

	view->setReferral(m_result.value());

	m_result.reset();

}


ReferralPresenter::ReferralPresenter(const Referral& r) :
	m_type(r.type), m_result(r), ref_rowid{ r.rowid }, lrn(r.lrn)
{
	
}

void ReferralPresenter::okPressed()
{
	auto common = view->getCommon();

	if (MKB::getNameFromMKBCode(common.mkbMain).empty()) {

		view->setErrorLabel("Моля, изберете основен код на диагнозата!");
		m_result.reset();
		return;
	}

	if (MKB::getNameFromMKBCode(common.mkbComorbMain).empty() && 
		MKB::getNameFromMKBCode(common.mkbComorbAdd).size())
	{
		view->setErrorLabel("Моля, изберете основен код на придружаващото заболяване!");
		m_result.reset();
		return;
	}

	m_result.emplace(m_type);

	m_result->rowid = ref_rowid;
	m_result->date = common.date;
	m_result->diagnosis.main = common.mkbMain;
	m_result->diagnosis.additional = common.mkbAdditional;
	m_result->comorbidity.main = common.mkbComorbMain;
	m_result->comorbidity.additional = common.mkbComorbAdd;
	m_result->number = common.number;
	m_result->reason = common.reason_idx;

	switch (m_type) {
	case ReferralType::MDD4:
		m_result->data = view->MDD4data(); break;
	case ReferralType::Form3:
		m_result->data = R3Data(); break;
	case ReferralType::Form3A:
		m_result->data = R3AData(); break;
	case ReferralType::MH119:
		m_result->data = view->MH119data(); break;
	}

	if (m_result->isNrnType()) {
		m_result->number = 0;
		m_result->lrn = lrn;
		view->closeDialog();
		return;
	}

	if (DbReferral::refNumberExists(m_type, common.date.year, ref_rowid) &&
		!ModalDialogBuilder::askDialog(
				"Направление с такъв номер вече съществува.\n"
				"Сигурни ли сте, че искате да дублирате номерацията?"
			)
		) 
	{
		m_result.reset(); 
		return;
	}

	view->closeDialog();
}


std::optional<Referral> ReferralPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return m_result;
}