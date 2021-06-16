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

    QRegExpValidator* phoneValidator;
    QRegExpValidator* numValidator;
    QRegExpValidator* nameValidator;


    Ui::PatientFormDialog ui;

    void paintEvent(QPaintEvent* event);

    PatientDialogPresenter* presenter;

    std::array<AbstractLineEdit*, 9> patientFields;

public:
    Q_OBJECT


public:

    PatientFormDialog(PatientDialogPresenter* p, QWidget* parent = 0);
    ~PatientFormDialog();

    void setEditMode(bool editMode);
    void close() override;
    void setLn4View(bool show);
    void setCodeInfo(const std::string& codeInfo) override;

    void resetFields() override;

    void setPatient(const Patient& patient) override;
    Patient getPatient() override;

    AbstractLineEdit* lineEdit(PatientField field) override;
};
