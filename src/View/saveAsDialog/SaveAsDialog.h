#pragma once

#include <QDialog>
#include <QLabel>
#include "Presenter/AmbListPagePresenter.h"
#include <map>
#include "MnogoTypSpinBox.h"
#include "ui_SaveAsDialog.h"


class SaveAsDialog : private QDialog
{

    int currentNumber;
    void okButtonPressed();
    std::map <int, bool> validNumber;
    void reject() override;
    AmbListPagePresenter* presenter;

public:
    Q_OBJECT
public:
    SaveAsDialog(QWidget *parent = Q_NULLPTR);
    ~SaveAsDialog();

    int exec(std::map <int, bool>validNumber, int currentNumber);

private:
    Ui::SaveAsDialog ui;
};