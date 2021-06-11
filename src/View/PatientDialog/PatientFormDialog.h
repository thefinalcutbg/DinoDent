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

public:
    Q_OBJECT

    PatientDialogPresenter presenter;
public:

    PatientFormDialog(QWidget* parent = 0);
    ~PatientFormDialog();

    void open(bool editMode);
    void close();
    void setLn4View(bool show);
    void accept() override;

    PatientDialogPresenter* Presenter();


};
