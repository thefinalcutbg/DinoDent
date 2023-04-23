#include "PriceListSettings.h"

PriceListSettings::PriceListSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	auto& table = ui.tableView;

	table->setModel(&m_model);

	table->hideColumn(0);
	table->setColumnWidth(1, 100);
	table->setColumnWidth(2, 370);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

	connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=] {

		if (presenter == nullptr) return;

		int idx = table->selectionModel()->currentIndex().row();

		if (idx == -1) {

			presenter->setCurrentIndex(idx);
			return;
		}

		int procedure = m_model.index(idx, 0).data().toInt();
		presenter->setCurrentIndex(procedure);
		});

	connect(ui.addButton, &QPushButton::clicked, this, [=] { if(presenter)presenter->addTemplate(); });
	//connect(ui.editButton, &QPushButton::clicked, this, [=] { if (presenter)presenter->editTemplate(); });
	connect(ui.deleteButton, &QPushButton::clicked, this, [=] { if (presenter)presenter->deleteTemplate(); });
	//connect(ui.tableView, &QTableView::doubleClicked, [=] { if (presenter)presenter->editTemplate(); });

}

void PriceListSettings::setPresenter(PriceListSettingsPresenter* presenter)
{
	this->presenter = presenter;
}

void PriceListSettings::setTemplates(const std::vector<ProcedureCode>& templates)
{
	//m_model.setProcedures(templates);
}

void PriceListSettings::setCurrentIndex(int idx)
{
	ui.tableView->selectColumn(idx);
}

PriceListSettings::~PriceListSettings()
{
}
