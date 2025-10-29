#include "ReferralPresenter.h"

#include "Database/DbReferral.h"

#include "Model/Dental/AmbList.h"
#include "Model/FreeFunctions.h"

#include "View/ModalDialogBuilder.h"
#include "View/Widgets/ReferralDialog.h"

ReferralPresenter::ReferralPresenter(const AmbList& sheet, ReferralType t) :
    lrn(FreeFn::getUuid()),
    m_type(t),
	ambSheetDate(sheet.getDate())
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


	if (ambSheetDate.isFromPreviousMonths(Date::currentDate())) {
		m_result->date = ambSheetDate;
	}
	
	date_validator.setMinDate(sheet.getDate());
	date_validator.setMaxDate(sheet.getDate().getMaxDateOfMonth());
	date_validator.setMinErrorMsg("Датата на направлението не може да е по-малка от тази на амбулаторния лист");
	date_validator.setMaxErrorMsg("Датата на направлението не може да е от следващ месец");
	
}

void ReferralPresenter::setView(ReferralDialog* view)
{
	this->view = view;

	view->setTitle(m_result->getTypeAsString());

	view->setDateValidator(&date_validator);

	view->setReferral(m_result.value());



	m_result.reset();

}


ReferralPresenter::ReferralPresenter(const AmbList& sheet, const Referral& r) :
    m_result(r), ref_rowid{ r.rowid }, lrn(r.lrn), m_type(r.type), sentToHis(r.isSentToHIS()), ambSheetDate(sheet.getDate())
{
	
}

void ReferralPresenter::okPressed()
{

	if (sentToHis) {
		ModalDialogBuilder::showMessage("Направлението вече е изпратено в НЗИС. Промените няма да бъдат запазени");
		m_result.reset();
		view->close();
		return;
	}

	auto common = view->getCommon();

	if (ICD10::getDescriptionFromICDCode(common.mkbMain).empty()) {

		view->setErrorLabel("Моля, изберете основен код на диагнозата!");
		m_result.reset();
		return;
	}

	if (ICD10::getDescriptionFromICDCode(common.mkbComorbMain).empty() && 
		ICD10::getDescriptionFromICDCode(common.mkbComorbAdd).size())
	{
		view->setErrorLabel("Моля, изберете основен код на придружаващото заболяване!");
		m_result.reset();
		return;
	}

	if (!common.date.isTheSameMonthAs(ambSheetDate))
	{
		ModalDialogBuilder::showMessage("Направлението трябва да е издадено в месеца в който е издаден амбулаторния лист");
		m_result.reset();
		return;
	}

	if (common.date < ambSheetDate)
	{
		ModalDialogBuilder::showMessage("Датата на направлението не може да е по-малка от тази на амбулаторния лист");
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
		view->close();
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

	view->close();
}

std::optional<Referral> ReferralPresenter::openDialog()
{
	ReferralDialog d(*this);
	d.exec();

	return m_result;
}
