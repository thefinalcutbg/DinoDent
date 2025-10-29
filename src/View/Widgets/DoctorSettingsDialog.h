#pragma once

#include <QDialog>

#include "ui_DoctorSettingsDialog.h"

#include "View/TableModels/SpecialtyTableModel.h"
#include "Model/UserStructs.h"

class DoctorDialogPresenter;

class DoctorSettingsDialog final : public QDialog
{
	Q_OBJECT

    DoctorDialogPresenter& presenter;

	LineEdit* lineEdits[6];

	SpecialtyTableModel his_specialtyModel;

	void paintEvent(QPaintEvent* event) override;

public:

	enum Field { LPK, FirstName, MiddleName, LastName, Phone, Password };

    DoctorSettingsDialog(DoctorDialogPresenter& presenter, QWidget *parent = Q_NULLPTR);
	void setDoctor(const Doctor& doctor);
	Doctor getDoctor();
	AbstractLineEdit* lineEdit(Field field)  { return lineEdits[field]; }
    void setToReadOnly();
	void close() { QWidget::close(); }

	~DoctorSettingsDialog();

private:
	Ui::DoctorSettingsDialog ui;
};
