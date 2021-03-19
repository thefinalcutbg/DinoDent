#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "ui_ProcedureDialog.h"

#include "IProcedureDialog.h"
#include "ProcedureModel.h"
#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

class ProcedureDialog : public QDialog, public IProcedureDialog
{
	Q_OBJECT

	ProcedureDialogPresenter presenter;

	ProcedureModel model;
	QSortFilterProxyModel* proxyModel;

	void resetForm();
public:
	ProcedureDialog(QWidget *parent = Q_NULLPTR);
	~ProcedureDialog();

	virtual void openProcedureDialog();
	virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList);
	
	virtual void showErrorMessage(const std::string& error);
	virtual void showErrorDialog(const std::string& error);
	virtual void setView(FormView view);
	virtual void close();
	virtual void setParameters(double price);
	virtual void setParameters(const std::string& material);
	virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum);
	virtual double getPrice();
	ProcedureDialogPresenter* getPresenter();

private:
	Ui::ProcedureDialog ui;
};
