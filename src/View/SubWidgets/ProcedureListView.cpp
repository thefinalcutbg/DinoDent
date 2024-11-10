#include "ProcedureListView.h"

#include "Model/Dental/ProcedureCode.h"

ProcedureListView::ProcedureListView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(3);

	auto& table = ui.tableView;

	table->setModel(&proxyModel);
	table->setMouseTracking(true);

	table->setColumnWidth(0, 30);
	table->hideColumn(1);
	table->setColumnWidth(2, 70);
	table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);

	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);


	table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	table->verticalHeader()->hide();

	auto favDelagete = new FavButtonDelegate();
	table->setItemDelegateForColumn(0, favDelagete);

	connect(favDelagete, &FavButtonDelegate::updateRequested, this, [&] {
		ui.tableView->viewport()->update();
	});

	connect(favDelagete, &FavButtonDelegate::favouriteClicked, this, [&](int rowIdx) {

		auto code = model.index(rowIdx, 1).data().toString().toStdString();

		presenter->favClicked(ui.sectionCombo->currentIndex(), code);

		refresh();

	});

	connect(ui.tableView, &QTableView::doubleClicked, this, [&](const QModelIndex& index) {
			
		emit codeDoubleClicked(
			proxyModel.index(s_idx, 1).data().toString().toStdString(),
			proxyModel.index(s_idx, 0).data(Qt::UserRole).toInt() == 1,
			proxyModel.index(s_idx, 3).data(Qt::UserRole).toDouble()
		);

	});

	connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {

		s_idx = table->selectionModel()->currentIndex().row();

		if (s_idx == -1) {
			emit codeSelected(std::string{}, 0, 0);
			ui.tableView->clearSelection(); //because of bug when reseting the model
			return;
		}

		emit codeSelected(
			proxyModel.index(s_idx, 1).data().toString().toStdString(),
			proxyModel.index(s_idx, 0).data(Qt::UserRole).toInt() == 1, //NHIF value from model
			proxyModel.index(s_idx, 3).data(Qt::UserRole).toDouble()
		);
		});


	connect(ui.searchEdit, &QLineEdit::textChanged, this, [=, this](const QString& text)
	{
			s_search = text;
			proxyModel.setFilterRegularExpression(QRegularExpression(s_search, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(s_idx);
	});

	

	connect(ui.sectionCombo, &QComboBox::currentIndexChanged, this, [&](int index) { refresh(); });
}

void ProcedureListView::setPresenter(ProcedureListPresenter* p)
{
	presenter = p;

	if (current_section == -1) {
		current_section = presenter->hasFavourites() ? 1 : 0;
	}

	ui.sectionCombo->clear();

	ui.sectionCombo->blockSignals(true);

	for (auto& name : p->getSectionList()) {
		ui.sectionCombo->addItem(name.c_str());
	}

	ui.sectionCombo->setItemIcon(1, QIcon(":/icons/icon_fav.png"));

	ui.sectionCombo->setCurrentIndex(current_section);

	ui.sectionCombo->blockSignals(false);

	refresh();

	ui.searchEdit->setText(s_search);
	ui.tableView->selectRow(s_idx);
}

void ProcedureListView::refresh()
{
	current_section = ui.sectionCombo->currentIndex();

	auto list = presenter->getList(current_section);

	auto hidePriceColumn = true;

	for (auto& p : list) {
		if (p.price && !p.nhif) {
			hidePriceColumn = false;
			break;
		}
	}

	ui.tableView->setColumnHidden(4, hidePriceColumn);

	model.setProcedures(presenter->getList(current_section), current_section < 2);

	emit codeSelected(std::string{}, false, 0);
}

