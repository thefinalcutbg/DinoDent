#pragma once

#include <QDialog>
#include "ui_ReferralDialog.h"
#include "Model/Referrals/ReferralData.h"
#include "View/TableModels/MKBModel.h"
#include "Model/Referrals/Referral.h"

class DateValidator;
class ReferralPresenter;


class ReferralDialog : public QDialog
{
	Q_OBJECT

    ReferralPresenter& presenter;

	MKBModel m_mkbFull{ ICD10::getFullMKBList() };
	MKBModel m_mkbDental{ ICD10::getDentalICDCodes() };

	void paintEvent(QPaintEvent* event) final;

	void setRefTypeView(ReferralType t);

public:

	struct CommonData {
		Date date;
		int number;
		std::string mkbMain;
		std::string mkbAdditional;
		std::string mkbComorbMain;
		std::string mkbComorbAdd;
		int reason_idx;
	};

    ReferralDialog(ReferralPresenter &p, QWidget *parent = nullptr);

	// Inherited via IReferralDialog
	void setTitle(const std::string& str);
	void setReferral(const Referral& referral);
	CommonData getCommon();
	MDD4Data MDD4data();
	MH119Data MH119data();
	void setErrorLabel(const std::string& str);
	void setDateValidator(DateValidator *d);

	~ReferralDialog();

private:
	Ui::ReferralDialogClass ui;




};
