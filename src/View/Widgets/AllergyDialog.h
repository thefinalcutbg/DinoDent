#pragma once

#include <QDialog>
#include "ui_AllergyDialog.h"
#include "Model/Allergy.h"


class AllergyDialog : public QDialog
{
	Q_OBJECT

	std::optional<Allergy> m_result;

	void paintEvent(QPaintEvent* event) override;

public:
	AllergyDialog(const Allergy& a);
	std::optional<Allergy> getResult() { return m_result; }
	~AllergyDialog();

private:
	Ui::AllergyDialog ui;
};
