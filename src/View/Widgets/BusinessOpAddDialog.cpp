#include "BusinessOpAddDialog.h"

BusinessOpAddDialog::BusinessOpAddDialog(const std::vector<ProcedureTemplate>& priceList, QWidget* parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Добавяне на услуга");

	m_priceList.setProcedures(priceList);

	proxyModel.setSourceModel(&m_priceList);
	proxyModel.setFilterKeyColumn(2);

	auto& table = ui.tableView;

	table->setModel(&proxyModel);

	table->hideColumn(0);
	table->setColumnWidth(1, 70);
	table->setColumnWidth(3, 70);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);

	connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
		[=]{
			s_idx = table->selectionModel()->currentIndex().row();
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { reject(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { this->okPressed(); });

	connect(ui.searchEdit, &QLineEdit::textChanged, [=]
		{
			s_search = ui.searchEdit->text();
			proxyModel.setFilterRegularExpression(QRegularExpression(s_search, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(s_idx);
		});


	connect(ui.tableView, &QTableView::doubleClicked, [=] { this->okPressed(); });
	
	ui.searchEdit->setText(s_search);
	ui.tableView->selectRow(s_idx);

}

void BusinessOpAddDialog::okPressed()
{
	if (s_idx == -1) return;

	auto index = ui.tableView->model()->data(ui.tableView->model()->index(s_idx, 0)).toInt();

	auto& row = m_priceList.getProcedureRow(index);

	m_result.emplace(std::to_string(row.code), row.name.toStdString(), row.price, ui.quantitySpinBox->value());

	accept();
}

BusinessOpAddDialog::~BusinessOpAddDialog()
{
}
