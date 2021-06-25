#pragma once

#include <QWidget>
#include <QMessageBox>

#include "ui_AmbListPage.h"
#include <QLabel>
#include <QScrollBar>
#include <vector>

#include "Presenter/AmbListPagePresenter.h"
#include "View/PatientDialog/PatientFormDialog.h"
#include "IAmbListPage.h"
#include "View/ListView/ListView.h"
#include "View/AmbListPage/saveAsDialog/SaveAsDialog.h"
#include "saveDialog/SaveDialog.h"

class AmbListPage : public QWidget, public IAmbListPage
{
    Q_OBJECT

    ListView list_view;
   
    AmbListPagePresenter presenter;
    

    SaveAsDialog saveAs_dialog;

public:
    AmbListPage(QWidget *parent = Q_NULLPTR);
    ~AmbListPage();

    ITabView* tabView() override;

    bool closeAllTabs();
    int openSaveAsDialog(int newNum, std::map<int, bool> existingNumbers);
    DialogAnswer openSaveDialog(std::string title);
    
private:
    Ui::AmbListPage ui;
};
