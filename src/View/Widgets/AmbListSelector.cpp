#include "AmbListSelector.h"
#include "Presenter/ListSelectorPresenter.h"
#include <QMessageBox>
#include <QDebug>
#include  <QRegularExpression>
#include <QApplication>

AmbListSelector::AmbListSelector(ListSelectorPresenter* presenter) :
	presenter(presenter)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Window);
	setWindowTitle(u8"Документи");
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


	connect(ui.dataTypeCombo, &QComboBox::currentIndexChanged,
		[=](int idx) {presenter->setListType(static_cast<TabType>(idx));});

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

	connect(ui.deleteButton, &QPushButton::clicked, 
		[=] 
		{
				presenter->deleteCurrentSelection();
		});

	
	connect(ui.newListButton, &QPushButton::clicked, [=] { presenter->openNew(TabType::AmbList);});
	connect(ui.newPerioButton, &QPushButton::clicked, [=] { presenter->openNew(TabType::PerioList);});
	connect(ui.newFinancialButton, &QPushButton::clicked, [=] { presenter->openNew(TabType::Financial);});

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
	setPatientMode(false);
	amb_model.setRows(rows);

	QSignalBlocker block(ui.dataTypeCombo);
	ui.dataTypeCombo->setCurrentIndex(0);

	idFilter.setSourceModel(&amb_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);
	
	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 80);
	ui.tableView->setColumnWidth(3, 80);
	ui.tableView->setColumnWidth(4, 250);
	ui.tableView->setColumnWidth(5, 120);
	
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
	setPatientMode(false);
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
	setPatientMode(true);
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
	setPatientMode(false);
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
	setPatientMode(false);
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

void AmbListSelector::setPatientMode(bool enable)
{
	ui.newFinancialButton->setHidden(!enable);
	ui.newListButton->setHidden(!enable);
	ui.newPerioButton->setHidden(!enable);

	enable ?
		ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection)
		:
		ui.tableView->setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
		    
}

void AmbListSelector::focus()
{
	activateWindow();
}

void AmbListSelector::close()
{
	QDialog::accept();
}

