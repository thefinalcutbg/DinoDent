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

	ProcedureDialogPresenter *presenter;

	ProcedureModel model;
	QSortFilterProxyModel* proxyModel;

	void paintEvent(QPaintEvent* event);
	void resetForm();
public:
	ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget* parent = Q_NULLPTR);
	virtual ~ProcedureDialog();

	virtual void openProcedureDialog() override;
	virtual void loadManipulationList(std::vector<ManipulationTemplate> manipulationList) override;
	
	virtual void showErrorMessage(const std::string& error) override;
	virtual void showErrorDialog(const std::string& error) override;
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
