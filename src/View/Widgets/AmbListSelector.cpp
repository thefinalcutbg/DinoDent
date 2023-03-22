#include "AmbListSelector.h"
#include "Presenter/ListSelectorPresenter.h"
#include <QMessageBox>
#include  <QRegularExpression>
#include <QApplication>
#include "View/Theme.h"
#include "QtVersion.h"
AmbListSelector::AmbListSelector(ListSelectorPresenter* presenter) :
	presenter(presenter)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Window);
	setWindowTitle("Документи");
	setWindowIcon(QIcon(":/icons/icon_open.png"));

	auto lambda = [=](QDate date) { return Date{ date.day(), date.month(), date.year() };};

	connect(ui.fromDateEdit, &QDateEdit::dateChanged,
		[=]() {presenter->setDates(lambda(ui.fromDateEdit->date()), lambda(ui.toDateEdit->date())); });
	connect(ui.toDateEdit, &QDateEdit::dateChanged,
		[=]() {presenter->setDates(lambda(ui.fromDateEdit->date()), lambda(ui.toDateEdit->date())); });

	connect(ui.openButton, &QPushButton::clicked, [=] {
		
		QApplication::setOverrideCursor(QCursor(Qt::CursorShape::WaitCursor));
		presenter->openCurrentSelection(); 
		QApplication::restoreOverrideCursor();
		
		});

	connect(ui.dataTypeCombo, QtComboIndexChanged, 
		[=](int idx) {presenter->setListType(static_cast<TabType>(idx)); });

	connect(ui.idSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.idSearchEdit->text();
			idFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		});

	connect(ui.nameSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.nameSearchEdit->text();
			nameFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		});

	connect(ui.phoneSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.phoneSearchEdit->text();
			phoneFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		});



	connect(ui.tableView, &QTableView::doubleClicked, this, [=] { presenter->openCurrentSelection(); });

	connect(ui.tableView, &ListTable::deletePressed, this, [=] { ui.deleteButton->click(); });

	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.tableView, &QTableView::customContextMenuRequested, this, [=](const QPoint& p) {contextMenuRequested(p);});

	connect(ui.deleteButton, &QPushButton::clicked, 
		[=] 
		{
				presenter->deleteCurrentSelection();
		});



	main_menu = new QMenu(this);

	QAction* action;

	action = (new QAction("Отвори", main_menu));
	connect(action, &QAction::triggered, [=] { presenter->openCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_open.png"));
	main_menu->addAction(action);

	sub_menu = new QMenu("Нов документ", this);

	//ui.printButton->setIcon(QIcon(":/icons/icon_print.png"));
	main_menu->addMenu(sub_menu);

	action = (new QAction("Нов амбулаторен лист", sub_menu));
	connect(action, &QAction::triggered, [=] { presenter->openNewDocument(TabType::AmbList); });
	action->setIcon(QIcon(":/icons/icon_sheet.png"));
	sub_menu->addAction(action);

	action = (new QAction("Ново пародонтално измерване", sub_menu));
	connect(action, &QAction::triggered, [=] { presenter->openNewDocument(TabType::PerioList); });
	action->setIcon(QIcon(":/icons/icon_periosheet.png"));
	sub_menu->addAction(action);

	action = (new QAction("Нова рецепта", sub_menu));
	connect(action, &QAction::triggered, [=] { presenter->openNewDocument(TabType::Prescription); });
	action->setIcon(QIcon(":/icons/icon_prescr.png"));
	sub_menu->addAction(action);

	action = (new QAction("Нова фактура", sub_menu));
	connect(action, &QAction::triggered, [=] { presenter->openNewDocument(TabType::Financial); });
	action->setIcon(QIcon(":/icons/icon_invoice.png"));
	sub_menu->addAction(action);

	action = (new QAction("Изтрий", main_menu));
	connect(action, &QAction::triggered, [=] { presenter->deleteCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_remove.png"));
	main_menu->addAction(action);

	main_menu->setStyleSheet(Theme::getPopupMenuStylesheet());
	sub_menu->setStyleSheet(Theme::getPopupMenuStylesheet());

	presenter->setView(this);

}

AmbListSelector::~AmbListSelector()
{
	ui.fromDateEdit->blockSignals(true);
	ui.toDateEdit->blockSignals(true);
	presenter->setView(nullptr);

}


void AmbListSelector::setDates(const Date& from, const Date& to)
{
	QSignalBlocker f(ui.fromDateEdit);
	QSignalBlocker t(ui.toDateEdit);
	ui.fromDateEdit->setDate(QDate(from.year, from.month, from.day));
	ui.toDateEdit->setDate(QDate(to.year, to.month, to.day));
}

void AmbListSelector::setRows(const std::vector<AmbRow>& rows)
{
	amb_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(0);

	idFilter.setSourceModel(&amb_model);
	idFilter.setFilterKeyColumn(4);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(5);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(6);

	ui.tableView->setModel(&phoneFilter);
	
	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 50);
	ui.tableView->setColumnWidth(2, 120);
	ui.tableView->setColumnWidth(3, 80);
	ui.tableView->setColumnWidth(4, 80);
	ui.tableView->setColumnWidth(5, 250);
	ui.tableView->setColumnWidth(6, 120);

	ui.fromDateEdit->setDisabled(false);
	ui.toDateEdit->setDisabled(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList){
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		this->presenter->selectionChanged(selectedIndexes);

		}

	);
}


void AmbListSelector::setRows(const std::vector<PerioRow>& rows)
{
	perio_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(1);
	
	idFilter.setSourceModel(&perio_model);
	idFilter.setFilterKeyColumn(2);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(3);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(4);

	ui.tableView->setModel(&phoneFilter);


	ui.tableView->hideColumn(0);
	
	ui.tableView->setColumnWidth(1, 80);
	ui.tableView->setColumnWidth(2, 80);
	ui.tableView->setColumnWidth(3, 250);
	ui.tableView->setColumnWidth(4, 120);
	
	ui.fromDateEdit->setDisabled(false);
	ui.toDateEdit->setDisabled(false);


	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		this->presenter->selectionChanged(selectedIndexes);

		

		}

	);
}


void AmbListSelector::setRows(const std::vector<PatientRow>& rows)
{
	patient_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(2);

	idFilter.setSourceModel(&patient_model);
	idFilter.setFilterKeyColumn(1);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(2);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(3);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	
	ui.tableView->setColumnWidth(1, 80);
	ui.tableView->setColumnWidth(2, 250);
	ui.tableView->setColumnWidth(3, 120);
	

	ui.fromDateEdit->setDisabled(true);
	ui.toDateEdit->setDisabled(true);


	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		this->presenter->selectionChanged(selectedIndexes);


		}

	);
}

void AmbListSelector::setRows(const std::vector<FinancialRow>& rows)
{
	financial_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(3);

	idFilter.setSourceModel(&financial_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 80);
	ui.tableView->setColumnWidth(3, 100);
	ui.tableView->setColumnWidth(4, 250);
	ui.tableView->setColumnWidth(5, 100);

	ui.fromDateEdit->setDisabled(false);
	ui.toDateEdit->setDisabled(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		this->presenter->selectionChanged(selectedIndexes);

		}

	);
}

void AmbListSelector::setRows(const std::vector<PrescriptionRow>& rows)
{
	perscription_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(4);

	idFilter.setSourceModel(&perscription_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 80);
	ui.tableView->setColumnWidth(3, 250);
	ui.tableView->setColumnWidth(4, 120);

	ui.fromDateEdit->setDisabled(false);
	ui.toDateEdit->setDisabled(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		this->presenter->selectionChanged(selectedIndexes);

		}

	);
}


void AmbListSelector::contextMenuRequested(const QPoint& p)
{
	if (ui.tableView->selectionModel()->currentIndex().row() == -1) return;

	sub_menu->setDisabled(ui.dataTypeCombo->currentIndex() == 3);

	main_menu->popup(ui.tableView->viewport()->mapToGlobal(p));
}

void AmbListSelector::focus()
{
	activateWindow();
}

void AmbListSelector::close()
{
	QDialog::accept();
}

