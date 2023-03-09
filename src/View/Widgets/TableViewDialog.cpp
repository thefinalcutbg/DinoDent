#include "TableViewDialog.h"



TableViewDialog::TableViewDialog(QAbstractTableModel& model, int column, const std::string& code, QWidget* parent) :
	m_model(model), m_column(column), QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::Window);

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

	connect(ui.searchEdit, &QLineEdit::textChanged, [=](const QString& text)
		{
			m_proxyModel.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { reject(); });
	connect(ui.okButton, &QPushButton::clicked, [=] {

		m_selectedRow = ui.tableView->selectionModel()->currentIndex().row();
	if (m_selectedRow > -1) accept();

		});
	connect(ui.tableView, &QTableView::doubleClicked, [=] { ui.okButton->click(); });

	int selectRow = -1;

	for (int i = 0; i < m_model.rowCount(); i++)
	{
		if (m_model.index(i, column).data().toString().toStdString() != code) continue;

		ui.tableView->selectRow(i);

	}
	
}


std::string TableViewDialog::getResult()
{
	if(result() == QDialog::Rejected) return {};

	return ui.tableView->model()->index(m_selectedRow, m_column)
		.data()
		.toString()
		.toStdString();
	
}

TableViewDialog::~TableViewDialog()
{

}
