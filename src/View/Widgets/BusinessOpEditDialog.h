#pragma once

#include <QDialog>
#include "ui_BusinessOpEditDialog.h"
#include "Model/Financial/BusinessOperation.h"
#include <optional>

class BusinessOpEditDialog : public QDialog
{
	Q_OBJECT

	std::optional<BusinessOperation> m_operation;

	void paintEvent(QPaintEvent* e) override;

public:
	BusinessOpEditDialog(const BusinessOperation& op, QWidget *parent = Q_NULLPTR);

	std::optional<BusinessOperation> getResult() { return m_operation; };

	~BusinessOpEditDialog();

	

private:
	Ui::BusinessOpEditDialog ui;
};
