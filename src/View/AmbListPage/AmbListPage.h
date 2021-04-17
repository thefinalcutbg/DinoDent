#pragma once

#include <QWidget>
#include <QMessageBox>

#include "ui_amblistpage.h"
#include <QLabel>
#include <QScrollBar>
#include <vector>

#include "Presenter/AmbListPagePresenter.h"
#include "View/PatientDialog/PatientFormDialog.h"
#include "IAmbListPage.h"
#include "View/ListView/ListView.h"
#include "view/AmbListPage/saveAsDialog/SaveAsDialog.h"
#include "saveDialog/SaveDialog.h"

class AmbListPage : public QWidget, public IAmbListPage
{
	Q_OBJECT

	Database* database;

	AmbListPagePresenter presenter;

	ListView list_view;

	int previousTabData;

	PatientFormDialog patient_dialog;
	SaveAsDialog saveAs_dialog;

public:
	AmbListPage(QWidget *parent = Q_NULLPTR);
	~AmbListPage();

	AmbListPagePresenter* Presenter();
	void newTab(int vecPos, std::string tabName);
	void focusTab(int vecPos);
	void removeCurrentTab();
	void changeTabName(std::string tabName);
	int openSaveAsDialog(int newNum, std::map<int, bool> existingNumbers);
	DialogAnswer openSaveDialog(std::string title);
	bool closeAllTabs();
private:
	Ui::AmbListPage ui;
};
