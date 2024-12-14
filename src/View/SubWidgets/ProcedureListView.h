#pragma once

#include <QWidget>
#include <QSortFilterProxyModel>
#include "Presenter/ProcedureListPresenter.h"
#include "View/TableModels/ProcedureTemplateModel.h"
#include "ui_ProcedureListView.h"

class ProcedureListPresenter;

class ProcedureListView : public QWidget
{
	Q_OBJECT
	int current_index{ -1 };
	inline static QString s_search{ "" };
	inline static int current_section{ -1 };

	ProcedureTemplateModel model;
	QSortFilterProxyModel proxyModel;

	bool eventFilter(QObject* obj, QEvent* event);

	ProcedureListPresenter* presenter{ nullptr };

public:
	ProcedureListView(QWidget *parent = nullptr);

	void setPresenter(ProcedureListPresenter* p);
	void refresh();
	~ProcedureListView() {};

signals:
	void deletePressed(const std::string& code);
	void codeSelected(const std::string& code, bool nhif, double price);
	void codeDoubleClicked(const std::string& code, bool nhif, double price);
	void favouritesClicked(const std::string& code);

private:
	Ui::ProcedureListViewClass ui;
};
