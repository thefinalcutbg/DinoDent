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

	MKBModel m_mkbDental{ MKB::getDentalMKBList() };
	NotEmptyValidator not_emmptyValidator;

	std::optional<MedicalNotice> m_result;

	void paintEvent(QPaintEvent* event) override;

public:
	MedicalNoticeDialog();

	std::optional<MedicalNotice> getResult() { return m_result; }

	~MedicalNoticeDialog();

private:
	Ui::MedicalNoticeDialog ui;
};
