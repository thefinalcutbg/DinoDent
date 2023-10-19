#include "BrowserDialog.h"
#include "Presenter/BrowserPresenter.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include "View/Theme.h"
#include "QtVersion.h"

BrowserDialog::BrowserDialog()
{
	ui.setupUi(this);

	ui.tabBar->addTab(QIcon(":/icons/icon_user.png"), "Пациенти");
	ui.tabBar->addTab(QIcon(":/icons/icon_sheet.png"), "Амбулаторни листи");
	ui.tabBar->addTab(QIcon(":/icons/icon_prescr.png"), "Рецепти");
	ui.tabBar->addTab(QIcon(":/icons/icon_periosheet.png"), "Пародонтални измервания");
	ui.tabBar->addTab(QIcon(":/icons/icon_invoice.png"), "Финансови документи");

	ui.tabBar->setExpanding(false);
	ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

	ui.frame->setStyleSheet("QFrame#frame{"
		"background-color: rgb(249,249,249);"
	"}");


	setWindowFlags(Qt::Window);
	setWindowTitle("Документи");
	setWindowIcon(QIcon(":/icons/icon_open.png"));

	auto lambda = [](const QDate& date) { return Date{ date.day(), date.month(), date.year() };};

	connect(ui.fromDateEdit, &QDateEdit::dateChanged,
		[&]() {presenter.setDates(lambda(ui.fromDateEdit->date()), lambda(ui.toDateEdit->date())); });
	connect(ui.toDateEdit, &QDateEdit::dateChanged,
		[=]() {presenter.setDates(lambda(ui.fromDateEdit->date()), lambda(ui.toDateEdit->date())); });

	connect(ui.openButton, &QPushButton::clicked, [=] {
		
		QApplication::setOverrideCursor(QCursor(Qt::CursorShape::WaitCursor));
		presenter.openCurrentSelection(); 
		QApplication::restoreOverrideCursor();
		
		});

	connect(ui.tabBar, &QTabBar::currentChanged, 
		[&](int idx) {presenter.setListType(static_cast<TabType>(idx)); });

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



	connect(ui.tableView, &QTableView::doubleClicked, this, [&] { presenter.openCurrentSelection(); });

	connect(ui.tableView, &ListTable::deletePressed, this, [=] { ui.deleteButton->click(); });

	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.tableView, &QTableView::customContextMenuRequested, this, [=](const QPoint& p) {contextMenuRequested(p);});

	connect(ui.deleteButton, &QPushButton::clicked, 
		[=] 
		{
				presenter.deleteCurrentSelection();
		});

	ui.nameSearchEdit->setFocus();

	presenter.setView(this);

	//ui.dataTypeCombo->setCurrentIndex(2);

}

BrowserDialog::~BrowserDialog()
{
}


void BrowserDialog::setDates(const Date& from, const Date& to)
{
	QSignalBlocker f(ui.fromDateEdit);
	QSignalBlocker t(ui.toDateEdit);
	ui.fromDateEdit->setDate(QDate(from.year, from.month, from.day));
	ui.toDateEdit->setDate(QDate(to.year, to.month, to.day));
}

void BrowserDialog::setRows(const std::vector<AmbRow>& rows)
{
	amb_model.setRows(rows);

	QSignalBlocker block(ui.tabBar);
	ui.tabBar->setCurrentIndex(static_cast<int>(TabType::AmbList));

	idFilter.setSourceModel(&amb_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);
	
	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 120);
	ui.tableView->setColumnWidth(3, 120);
	ui.tableView->setColumnWidth(4, 240);
	ui.tableView->setColumnWidth(5, 120);

	hideRanges(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList){
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

			presenter.selectionChanged(selectedIndexes);

		}

	);
}


void BrowserDialog::setRows(const std::vector<PerioRow>& rows)
{
	perio_model.setRows(rows);

	QSignalBlocker block(ui.tabBar);
	ui.tabBar->setCurrentIndex(static_cast<int>(TabType::PerioStatus));
	
	idFilter.setSourceModel(&perio_model);
	idFilter.setFilterKeyColumn(2);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(3);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(4);

	ui.tableView->setModel(&phoneFilter);


	ui.tableView->hideColumn(0);
	
	ui.tableView->setColumnWidth(1, 80);
	ui.tableView->setColumnWidth(2, 150);
	ui.tableView->setColumnWidth(3, 250);
	ui.tableView->setColumnWidth(4, 120);
	
	hideRanges(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		presenter.selectionChanged(selectedIndexes);

		

		}

	);
}


void BrowserDialog::setRows(const std::vector<PatientRow>& rows)
{
	patient_model.setRows(rows);

	QSignalBlocker block(ui.tabBar);
	ui.tabBar->setCurrentIndex(static_cast<int>(TabType::PatientSummary));

	idFilter.setSourceModel(&patient_model);
	idFilter.setFilterKeyColumn(1);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(2);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(3);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	
	ui.tableView->setColumnWidth(1, 150);
	ui.tableView->setColumnWidth(2, 250);
	ui.tableView->setColumnWidth(3, 120);
	
	hideRanges(true);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

		presenter.selectionChanged(selectedIndexes);


		}

	);
}

void BrowserDialog::setRows(const std::vector<FinancialRow>& rows)
{
	financial_model.setRows(rows);

	QSignalBlocker block(ui.tabBar);
	ui.tabBar->setCurrentIndex(static_cast<int>(TabType::Financial));

	idFilter.setSourceModel(&financial_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 100);
	ui.tableView->setColumnWidth(3, 100);
	ui.tableView->setColumnWidth(4, 250);
	ui.tableView->setColumnWidth(5, 100);

	hideRanges(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

			presenter.selectionChanged(selectedIndexes);

		}

	);
}

void BrowserDialog::setRows(const std::vector<PrescriptionRow>& rows)
{
	prescription_model.setRows(rows);

	QSignalBlocker block(ui.tabBar);
	ui.tabBar->setCurrentIndex(static_cast<int>(TabType::Prescription));

	idFilter.setSourceModel(&prescription_model);
	idFilter.setFilterKeyColumn(3);
	nameFilter.setSourceModel(&idFilter);
	nameFilter.setFilterKeyColumn(4);
	phoneFilter.setSourceModel(&nameFilter);
	phoneFilter.setFilterKeyColumn(5);

	ui.tableView->setModel(&phoneFilter);

	ui.tableView->hideColumn(0);
	ui.tableView->setColumnWidth(1, 100);
	ui.tableView->setColumnWidth(2, 120);
	ui.tableView->setColumnWidth(3, 100);
	ui.tableView->setColumnWidth(4, 250);
	ui.tableView->setColumnWidth(5, 120);

	hideRanges(false);

	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=] {

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(phoneFilter.index(idx.row(), 0).data().toInt());
		}

			presenter.selectionChanged(selectedIndexes);

		}

	);
}


void BrowserDialog::contextMenuRequested(const QPoint& p)
{
	if (ui.tableView->selectionModel()->currentIndex().row() == -1) return;

	if (main_menu) {
		delete main_menu;
	}

	main_menu = new QMenu(this);

	QAction* action;

	action = (new QAction("Отвори", main_menu));
	connect(action, &QAction::triggered, [=] { presenter.openCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_open.png"));
	main_menu->addAction(action);


	if (ui.tabBar->currentIndex() != 4) {
		action = (new QAction("Нов амбулаторен лист", main_menu));
		connect(action, &QAction::triggered, [=] { presenter.openNewDocument(TabType::AmbList); });
		action->setIcon(QIcon(":/icons/icon_sheet.png"));
		main_menu->addAction(action);

		action = (new QAction("Ново пародонтално измерване", main_menu));
		connect(action, &QAction::triggered, [=] { presenter.openNewDocument(TabType::PerioStatus); });
		action->setIcon(QIcon(":/icons/icon_periosheet.png"));
		main_menu->addAction(action);

		action = (new QAction("Нова рецепта", main_menu));
		connect(action, &QAction::triggered, [=] { presenter.openNewDocument(TabType::Prescription); });
		action->setIcon(QIcon(":/icons/icon_prescr.png"));
		main_menu->addAction(action);

		action = (new QAction("Нова фактура", main_menu));
		connect(action, &QAction::triggered, [=] { presenter.openNewDocument(TabType::Financial); });
		action->setIcon(QIcon(":/icons/icon_invoice.png"));
		main_menu->addAction(action);
	}

	action = (new QAction("Изтрий", main_menu));
	connect(action, &QAction::triggered, [=] { presenter.deleteCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_remove.png"));
	main_menu->addAction(action);

	action = (new QAction("Копирай текста", main_menu));
	connect(action, &QAction::triggered, [=] { 

		QClipboard* clipboard = QApplication::clipboard();
		QString text = ui.tableView->currentIndex().data().toString();
		clipboard->setText(text);
	});
	//action->setIcon(QIcon(":/icons/icon_remove.png"));
	main_menu->addAction(action);

	main_menu->setStyleSheet(Theme::getPopupMenuStylesheet());


	main_menu->popup(ui.tableView->viewport()->mapToGlobal(p));
}

void BrowserDialog::hideRanges(bool hidden)
{
	ui.fromLabel->setHidden(hidden);
	ui.toLabel->setHidden(hidden);
	ui.fromDateEdit->setHidden(hidden);
	ui.toDateEdit->setHidden(hidden);
}

void BrowserDialog::focus()
{
	activateWindow();
}

void BrowserDialog::close()
{
	QDialog::accept();
}

