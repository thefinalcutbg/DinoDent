﻿#include "KSMPDialog.h"


KSMPDialog::KSMPDialog(KsmpList& list, const std::string& code, QWidget* parent) :
	m_model(list), QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window);
	setWindowTitle(u8"Класификация на Медицинските Процедури");

	m_proxyModel.setSourceModel(&m_model);
	m_proxyModel.setFilterKeyColumn(1);

	ui.tableView->setModel(&m_proxyModel);
	
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableView->horizontalHeader()->setHighlightSections(false);
	ui.tableView->verticalHeader()->setVisible(false);
	ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableView->verticalHeader()->setDefaultSectionSize(20);
	ui.tableView->setWordWrap(false);
	ui.tableView->setColumnWidth(0, 70);
	//ui.tableView->setColumnWidth(1, 200);
	ui.tableView->setColumnWidth(2, 50);
	ui.tableView->setColumnWidth(3, 150);
	ui.tableView->setColumnWidth(4, 50);
	ui.tableView->setColumnWidth(5, 150);
	ui.tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=] {

		m_selectedRow = ui.tableView->selectionModel()->currentIndex().row();

		});

	connect(ui.searchEdit, &QLineEdit::textChanged, [=](const QString &text)
		{
			m_proxyModel.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(m_selectedRow);
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { reject(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { if(m_selectedRow > -1) accept(); });
	connect(ui.tableView, &QTableView::doubleClicked, [=] { ui.okButton->click(); });
	int selectRow = m_model.getRowFromCode(code);

	ui.tableView->selectRow(selectRow);
}

std::string KSMPDialog::getResult()
{
	if(result() == QDialog::Rejected) return {};

	return ui.tableView->model()->data(
		ui.tableView->model()->index(m_selectedRow, 0)
	)
	.toString().toStdString();
	
}

KSMPDialog::~KSMPDialog()
{}