#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>

#include "Presenter/PatientDialogPresenter.h"
#include "ui_PatientFormDialog.h"
#include "View/Interfaces/IPatientDialog.h"


#include "Model/Patient.h"

class PatientFormDialog : public QDialog, public IPatientDialog
{

    QRegularExpressionValidator* phoneValidator;
    QRegularExpressionValidator* numValidator;
    QRegularExpressionValidator* nameValidator;


    Ui::PatientFormDialog ui;

    void paintEvent(QPaintEvent* event) override;

    PatientDialogPresenter& presenter;

    std::array<LineEdit*, PatientField::size> patientFields;

public:
    Q_OBJECT


public:

    PatientFormDialog(PatientDialogPresenter& p, QWidget* parent = 0);
    ~PatientFormDialog();

    void setEditMode(bool editMode) override;
    void close() override;
    void setType(Patient::Type type) override;
    void setTitle(const std::string& title) override;
    void resetFields() override;

    void setPatient(const Patient& patient) override;
    Patient getPatient() override;
    void setHirbno(const std::string& hirbno) override;
    AbstractLineEdit* lineEdit(PatientField field) override;
    AbstractDateEdit* dateEdit() override;
};
