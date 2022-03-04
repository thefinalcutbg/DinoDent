#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "ui_ProcedureDialog.h"

#include "IProcedureDialog.h"
#include "View/ProcedureDisplayModel/ProcedureTemplateModel.h"

class ProcedureDialogPresenter;

class ProcedureDialog : public QDialog, public IProcedureDialog
{
	Q_OBJECT

	inline static int s_idx{ -1 };
	inline static QString s_search{""};

	ProcedureDialogPresenter *presenter;

	ProcedureTemplateModel model;
	QSortFilterProxyModel proxyModel;

public:
	ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget* parent = Q_NULLPTR);
	 ~ProcedureDialog();


	void setProcedureTemplates(std::vector<ProcedureTemplate> manipulationList) override;
	void showErrorMessage(const std::string& error) override;
	void setView(ProcedureTemplateType t) override;
    void close() override;
	void setSelectionLabel(const std::vector<int>& selectedTeethNum) override;
	void setObturationPresenter(ObturationPresenter* presenter) override;
	void setCrownPresenter(CrownPresenter* presenter) override;
	void setImplantPresenter(ImplantPresenter* presenter);
	void setFiberSplintPresenter(FiberSplintPresenter* presenter) override;
	ICommonFields* commonFields() override;


private:
	Ui::ProcedureDialog ui;
};
