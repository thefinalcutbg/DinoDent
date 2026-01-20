#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPainter>

#include "Presenter/PatientDialogPresenter.h"
#include "ui_PatientFormDialog.h"

#include "Model/Validators/PatientValidators.h"
#include "Model/Validators/DateValidator.h"
#include "Model/Validators/NameValidator.h"
#include "Model/Patient.h"

struct PatientFormDialog : public QDialog
{

    QRegularExpressionValidator* phoneValidator;
    QRegularExpressionValidator* numValidator;
    QRegularExpressionValidator* nameValidator;

    EgnValidator egn_validator;
    Ln4Validator ln4_validator;
    SSNValidator ssn_validator;
    NotEmptyValidator notEmpty_validator;
    DateValidator birth_validator;
    NameValidator name_validator;
    HIRBNoValidator hirb_validator;
    CityValidator city_validator;
    CyrillicValidator cyrillic_validator;
	EmailValidator email_validator;

    Ui::PatientFormDialog ui;

    void paintEvent(QPaintEvent* event) override;

    void patientTypeChanged(int patientType);

    PatientDialogPresenter& presenter;

    enum PatientField { id, fname, mname, lname, city, address, hirbno, phone, email, birthdate, size };

    std::array<AbstractUIElement*, PatientField::size> patientFields;

public:
    Q_OBJECT


public:

    PatientFormDialog(PatientDialogPresenter& p, QWidget* parent = 0);
    ~PatientFormDialog();

    void setEditMode(bool editMode);
    void setTitle(const std::string& title);
    void resetFields();

    void setPatient(const Patient& patient);
    Patient getPatient();
    void setHirbno(const std::string& hirbno);
    bool inputFieldsAreValid();
};
