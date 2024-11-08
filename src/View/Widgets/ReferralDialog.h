#pragma once

#include <QDialog>
#include "ui_ReferralDialog.h"
#include "Model/Referrals/ReferralData.h"
#include "View/Interfaces/IReferralDialog.h"
#include "View/TableModels/MKBModel.h"

class DateValidator;
class ReferralPresenter;


class ReferralDialog : public QDialog, public IReferralDialog
{
	Q_OBJECT

    ReferralPresenter& presenter;

	MKBModel m_mkbFull{ ICD10::getFullMKBList() };
	MKBModel m_mkbDental{ ICD10::getDentalICDCodes() };

	void paintEvent(QPaintEvent* event) final;

	void setRefTypeView(ReferralType t);

public:
    ReferralDialog(ReferralPresenter &p, QWidget *parent = nullptr);

	// Inherited via IReferralDialog
	void setTitle(const std::string& str) final;
	void setReferral(const Referral& referral) final;
	IReferralDialog::CommonData getCommon() final;
	MDD4Data MDD4data() final;
	MH119Data MH119data() final;
	void setErrorLabel(const std::string& str) final;
	void closeDialog()  final { close(); }
	void setDateValidator(DateValidator *d) final;

	~ReferralDialog();

private:
	Ui::ReferralDialogClass ui;




};
