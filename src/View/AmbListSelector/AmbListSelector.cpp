#include "AmbListSelector.h"
#include "Presenter/ListSelector/ListSelectorPresenter.h"
#include <QMessageBox>
#include <QDebug>
#include  <QRegularExpression>

AmbListSelector::AmbListSelector(ListSelectorPresenter* presenter) :
	presenter(presenter)
{
	ui.setupUi(this);

	setWindowFlags(Qt::Window);
	setWindowTitle(u8"Документи");
	setWindowIcon(QIcon(":/icons/icon_open.png"));


	connect(ui.fromDateEdit, &QDateEdit::dateChanged,
		[=]() {presenter->setDates(ui.fromDateEdit->getDate(), ui.toDateEdit->getDate()); });
	connect(ui.toDateEdit, &QDateEdit::dateChanged,
		[=]() {presenter->setDates(ui.fromDateEdit->getDate(), ui.toDateEdit->getDate()); });

	connect(ui.openButton, &QPushButton::clicked, [=] {presenter->openCurrentSelection(); });

	connect(ui.ambRadio, &QRadioButton::clicked, [=] {presenter->setListType(RowModelType::AmbListRow);});
	connect(ui.patientRadio, &QRadioButton::clicked, [=] {presenter->setListType(RowModelType::PatientRow);});
	connect(ui.perioRadio, &QRadioButton::clicked, [=] {presenter->setListType(RowModelType::PerioRow);});

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

	QSignalBlocker block(ui.ambRadio);
	ui.ambRadio->setChecked(true);

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
	perio_model.setRows(rows);

	QSignalBlocker block(ui.perioRadio);
	ui.perioRadio->setChecked(true);
	
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

	QSignalBlocker block(ui.patientRadio);
	ui.patientRadio->setChecked(true);

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



void AmbListSelector::focus()
{
	activateWindow();
}

void AmbListSelector::close()
{
	QDialog::accept();
}

