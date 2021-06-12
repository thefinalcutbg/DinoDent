#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QPainter>

#include "Presenter/PatientDialog/PatientDialogPresenter.h"
#include "ui_PatientFormDialog.h"
#include "IPatientDialog.h"
#include "Database/Database.h"

class PatientFormDialog : public QDialog, public IPatientDialog
{

    QRegExpValidator* phoneValidator;
    QRegExpValidator* numValidator;
    QRegExpValidator* nameValidator;


    Ui::PatientFormDialog ui;

    void paintEvent(QPaintEvent* event);

    PatientDialogPresenter* presenter;
public:
    Q_OBJECT


public:

    PatientFormDialog(PatientDialogPresenter* p, QWidget* parent = 0);
    ~PatientFormDialog();

    void setEditMode(bool editMode);
    void close();
    void setLn4View(bool show);
    void accept() override;


};
