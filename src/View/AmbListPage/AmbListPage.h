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


class AmbListPage : public QWidget, public IAmbListPage
{
    Q_OBJECT

    ListView list_view;
   
    AmbListPagePresenter presenter;
    


public:
    AmbListPage(QWidget *parent = Q_NULLPTR);
    ~AmbListPage();

    ITabView* tabView() override;

    bool closeAllTabs();
    
private:
    Ui::AmbListPage ui;
};
