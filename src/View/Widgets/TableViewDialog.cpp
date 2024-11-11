#include "TableViewDialog.h"



TableViewDialog::TableViewDialog(QAbstractTableModel& model, int column, const std::string& code, QWidget* parent) :
    QDialog(parent), m_column(column), m_model(model)
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

    connect(ui.searchEdit, &QLineEdit::textChanged, this, [=, this](const QString& text)
		{
			m_proxyModel.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption));
		});

    connect(ui.cancelButton, &QPushButton::clicked, this, [=, this] { reject(); });
    connect(ui.okButton, &QPushButton::clicked, this, [=, this] {

		m_selectedRow = ui.tableView->selectionModel()->currentIndex().row();
		if (m_selectedRow > -1) accept();

	});
    connect(ui.tableView, &QTableView::doubleClicked, this, [=, this] { ui.okButton->click(); });

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
