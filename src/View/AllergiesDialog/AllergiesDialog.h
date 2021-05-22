#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>


#include "ui_AllergiesDialog.h"
#include "IAllergiesDialog.h"
#include "Presenter/AllergiesDialog/AllergiesDialogPresenter.h"

class AllergiesDialog : public QDialog, IAllergiesDialog
{
    Q_OBJECT

    AllergiesDialogPresenter presenter;

    void paintEvent(QPaintEvent* event);

public:
    AllergiesDialog(QWidget* parent = Q_NULLPTR);
    void open(std::string allergies, std::string current, std::string past);
    void close();
    AllergiesDialogPresenter* getPresenter();
    ~AllergiesDialog();

private:
    Ui::AllergiesDialog ui;
};
