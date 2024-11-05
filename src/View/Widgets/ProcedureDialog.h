#pragma once

#include <QDialog>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include "ui_ProcedureDialog.h"

#include "View/Interfaces/IProcedureDialog.h"
#include "View/TableModels/ProcedureTemplateModel.h"

class ProcedureDialogPresenter;

class ProcedureDialog : public QDialog, public IProcedureDialog
{
	Q_OBJECT

	inline static int s_idx{ -1 };
	inline static QString s_search{""};

    ProcedureDialogPresenter &presenter;

	ProcedureTemplateModel model;
	QSortFilterProxyModel proxyModel;

public:
    ProcedureDialog(ProcedureDialogPresenter& presenter, QWidget* parent = Q_NULLPTR);
	 ~ProcedureDialog();
	
	void setProcedureSections(const std::vector<std::string>& sectionNames, int defaultIdx) override;
	void setProcedureTemplates(std::vector<ProcedureListElement> procedureList) override;
	

    void close() override;
	void setSelectionLabel(const std::vector<int>& selectedTeethNum) override;

	IProcedureInput* procedureInput() override;


private:
	Ui::ProcedureDialog ui;

};
