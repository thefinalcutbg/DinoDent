#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "ui_ProcedureDialog.h"

#include "IProcedureDialog.h"
#include "ProcedureModel.h"

class ProcedureDialogPresenter;

class ProcedureDialog : public QDialog, public IProcedureDialog
{
	Q_OBJECT

	ProcedureDialogPresenter *presenter;

	ProcedureModel model;
	QSortFilterProxyModel* proxyModel;

	void resetForm();
public:
	ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget* parent = Q_NULLPTR);
	virtual ~ProcedureDialog();


	virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) override;
	
	virtual void showErrorMessage(const std::string& error) override;
	virtual void setView(ManipulationType t) override;
	virtual void close() override;
	virtual void setSelectionLabel(const std::vector<int>& selectedTeethNum) override;
	virtual void setObturationPresenter(ObturationPresenter* presenter) override;
	virtual void setCrownPresenter(CrownPresenter* presenter) override;
	virtual void setImplantPresenter(ImplantPresenter* presenter);
	virtual ICommonFields* commonFields() override;


private:
	Ui::ProcedureDialog ui;
};
