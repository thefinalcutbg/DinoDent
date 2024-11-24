#pragma once

#include <QDialog>
#include "ui_MedicationTemplateDialog.h"
#include "View/TableModels/MedicationTableModel.h"
#include "Model/Prescription/Medication.h"

class MedicationTemplateDialog : public QDialog
{
	Q_OBJECT

	MedicationTableModel m_model;

	bool eventFilter(QObject* object, QEvent* event) override;

	std::vector<std::pair<long long, Medication>> m_templates;

	void removeTemplate();

public:
	MedicationTemplateDialog(QWidget *parent = nullptr);
	std::optional<Medication> getResult();
	~MedicationTemplateDialog();

private:
	Ui::MedicationTemplateDialogClass ui;
};
