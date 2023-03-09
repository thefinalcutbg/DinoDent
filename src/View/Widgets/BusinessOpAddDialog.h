#pragma once

#include <QDialog>
#include <QSortFilterProxyModel>
#include "ui_BusinessOpAddDialog.h"

#include <vector>
#include <optional>

#include "Model/Dental/ProcedureCode.h"
#include "Model/Financial/BusinessOperation.h"
#include "View/TableModels/ProcedureTemplateModel.h"

class BusinessOpAddDialog : public QDialog
{
	Q_OBJECT


	inline static int s_idx{ -1 };
	inline static QString s_search{ "" };

	ProcedureTemplateModel m_priceList;
	QSortFilterProxyModel proxyModel;

	std::optional<BusinessOperation> m_result;

	void okPressed();

public:
	BusinessOpAddDialog(const std::vector<ProcedureCode>& priceList, QWidget *parent = Q_NULLPTR);
	auto getResult() const { return m_result; }
	~BusinessOpAddDialog();

private:
	Ui::BusinessOpAddDialog ui;
};
