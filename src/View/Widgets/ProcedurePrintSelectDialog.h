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
    bool eventFilter(QObject* obj, QEvent* event) override;

	void showFinancingSourceButtonsLogic(const std::vector<Procedure>& procedures);

public:
	//if empty referral vector (as oposed to empty optional) is passed, ambulatory sheet format is assumed
	ProcedurePrintSelectDialog(const std::vector<Procedure>& procedures, bool ambListPrintSelect = false);
	
	const std::vector<int> selectedProcedures() const;
	bool printReferrals() const;
	void selectFinancingSource(FinancingSource source);
	bool printNhifAmbSheet() const {
		return !ui.nhifRadio->isHidden() && ui.nhifRadio->isChecked();
	};

	~ProcedurePrintSelectDialog();


private:
	Ui::ProcedurePrintSelectDialog ui;
};
