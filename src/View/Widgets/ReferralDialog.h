#pragma once

#include <QDialog>
#include "ui_ReferralDialog.h"
#include "Model/Referrals/ReferralData.h"
#include "View/Interfaces/IReferralDialog.h"

class ReferralPresenter;


class ReferralDialog : public QDialog, public IReferralDialog
{
	Q_OBJECT

		ReferralPresenter* presenter;

	void paintEvent(QPaintEvent* event) override;

	void setRefTypeView(ReferralType t);

public:
	ReferralDialog(ReferralPresenter *p, QWidget *parent = nullptr);

	// Inherited via IReferralDialog
	void setTitle(const std::string& str) override;
	void setReferral(const Referral& referral) override;
	IReferralDialog::CommonData getCommon() override;
	MDD4Data MDD4data() override;
	MH119Data MH119data() override;
	void setErrorLabel(const std::string& str) override;
	void closeDialog()  override { close(); }

	~ReferralDialog();

private:
	Ui::ReferralDialogClass ui;




};
