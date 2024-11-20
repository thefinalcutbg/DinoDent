#pragma once

#include <QDialog>
#include "ui_BusinessOperationDialog.h"
#include "Model/Financial/BusinessOperation.h"
#include "Model/Validators/CommonValidators.h"
#include <optional>

class BuisnessOperationDialog : public QDialog
{
	Q_OBJECT

	std::optional<BusinessOperation> m_operation;

	NotEmptyValidator notEmptyValidator;

	void paintEvent(QPaintEvent* e) override;

public:
	BuisnessOperationDialog(const std::optional<BusinessOperation>& op = {});

	std::optional<BusinessOperation> getResult() { return m_operation; };

	~BuisnessOperationDialog();

	

private:
	Ui::BusinessOperationDialog ui;
};
