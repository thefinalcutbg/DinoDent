#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QPainter>

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "ui_PatientFormDialog.h"
#include "IPatientDialog.h"


#include "Model/Patient.h"

class PatientFormDialog : public QDialog, public IPatientDialog
{

    QRegularExpressionValidator* phoneValidator;
    QRegularExpressionValidator* numValidator;
    QRegularExpressionValidator* nameValidator;


    Ui::PatientFormDialog ui;

    void paintEvent(QPaintEvent* event) override;

    PatientDialogPresenter* presenter;

    std::array<LineEdit*, 8> patientFields;

public:
    Q_OBJECT


public:

    PatientFormDialog(PatientDialogPresenter* p, QWidget* parent = 0);
    ~PatientFormDialog();

    void setEditMode(bool editMode) override;
    void close() override;
    void setLn4View(bool show) override;
    void setCodeInfo(const std::string& codeInfo) override;

    void resetFields() override;

    void setPatient(const Patient& patient) override;
    Patient getPatient() override;
    void setHirbno(const std::string& hirbno) override;
    void setInsuranceStatus(Insured status) override;
    AbstractLineEdit* lineEdit(PatientField field) override;
    AbstractDateEdit* dateEdit() override;
};
