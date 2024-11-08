#pragma once

#include <QDialog>
#include "ui_MedicalNoticeDialog.h"

#include <optional>

#include "View/TableModels/MKBModel.h"

#include "Model/Validators/CommonValidators.h"
#include "Model/MedicalNotice.h"

class MedicalNoticeDialog : public QDialog
{
	Q_OBJECT

	MKBModel m_mkbDental{ ICD10::getDentalICDCodes() };
	NotEmptyValidator not_emptyValidator;

	std::optional<MedicalNotice> m_result;

	std::string m_nrn;
	std::string m_lrn;

	void paintEvent(QPaintEvent* event) override;

public:
	MedicalNoticeDialog(const MedicalNotice& m);

	std::optional<MedicalNotice> getResult() { return m_result; }

	~MedicalNoticeDialog();

private:
	Ui::MedicalNoticeDialog ui;
};
