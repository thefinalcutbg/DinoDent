#pragma once

#include <QDialog>
#include "ui_ProcedurePrintSelectDialog.h"
#include <vector>
#include <optional>
#include "Model/Dental/Procedure.h"
#include "Model/Referrals/Referral.h"
#include "View/TableModels/ProcedureSelectModel.h"
#include <QRadioButton>

class ProcedurePrintSelectDialog : public QDialog
{
	Q_OBJECT

	std::vector<int> m_selectedRows;
	ProcedureSelectModel model;

	std::vector<QRadioButton*> mdd4_buttons;
	std::vector<ReferralType> ref_typeIndexes;
	int mh119index{ -1 };

	void paintEvent(QPaintEvent* e) override;

public:
	ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, const std::vector<Referral>& referrals = {}, QWidget* parent = Q_NULLPTR);
	
	const std::vector<int> selectedProcedures() const;
	bool printReferrals() const;
	void selectFinancingSource(FinancingSource source);

	~ProcedurePrintSelectDialog();


private:
	Ui::ProcedurePrintSelectDialog ui;
};
