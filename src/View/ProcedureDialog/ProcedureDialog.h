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

	virtual void openProcedureDialog() override;
	virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) override;
	
	virtual void showErrorMessage(const std::string& error) override;
	virtual void showErrorDialog(const std::string& error) override;
	virtual void setView(ManipulationType t) override;
	virtual void close() override;
	virtual void setParameters(double price) override;
	virtual void setParameters(const std::string& material) override;
	virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) override;
	virtual double getPrice() override;
	ProcedureDialogPresenter* getPresenter();

private:
	Ui::ProcedureDialog ui;
};
