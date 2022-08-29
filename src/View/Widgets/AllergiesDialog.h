#pragma once

#include <QDialog>
#include <QTextEdit>
#include <QKeyEvent>
#include <QPainter>
#include <QPushButton>


#include "ui_AllergiesDialog.h"
#include "View/Interfaces/IAllergiesDialog.h"
#include "Presenter/AllergiesDialogPresenter.h"

class AllergiesDialog : public QDialog, IAllergiesDialog
{
    Q_OBJECT

    AllergiesDialogPresenter* presenter;

    void paintEvent(QPaintEvent* event);

public:
    AllergiesDialog(AllergiesDialogPresenter* p);

    void setData(const Allergies& allergies) override;
    Allergies getData()override;
    void close() override;


    ~AllergiesDialog();

private:
    Ui::AllergiesDialog ui;


};
