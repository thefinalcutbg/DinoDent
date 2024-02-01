#pragma once

#include <QDialog>
#include "ui_DoctorSettingsDialog.h"

#include "View/Interfaces/IDoctorSettingsDialog.h"
#include "View/TableModels/SpecialtyTableModel.h"

class DoctorSettingsDialog final : public QDialog, public IDoctorSettingsDialog
{
	Q_OBJECT

    DoctorDialogPresenter& presenter;

	LineEdit* lineEdits[6];

	SpecialtyTableModel his_specialtyModel;

	void paintEvent(QPaintEvent* event) override;

public:
    DoctorSettingsDialog(DoctorDialogPresenter& presenter, QWidget *parent = Q_NULLPTR);

	void setDoctor(const Doctor& doctor) override;
	Doctor getDoctor() override;
	AbstractLineEdit* lineEdit(DoctorFields::Field field) override { return lineEdits[field]; }
    void setToReadOnly() override;
	void close() override { QWidget::close(); }

	~DoctorSettingsDialog();

private:
	Ui::DoctorSettingsDialog ui;
};
