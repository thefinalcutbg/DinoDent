#include "BrowserDialog.h"
#include "Presenter/BrowserPresenter.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include "View/Theme.h"
#include "QtVersion.h"
#include "View/ModalDialogBuilder.h"

BrowserDialog::BrowserDialog()
{
	ui.setupUi(this);

	//ui.preView->hide();
	ui.preView->horizontalHeader()->setStretchLastSection(true);
	//ui.preView->verticalHeader()->setFixedHeight(20);

	idFilter.setSourceModel(&table_model);
	nameFilter.setSourceModel(&idFilter);
	phoneFilter.setSourceModel(&nameFilter);
	ui.tableView->setModel(&phoneFilter);
	ui.preView->setModel(&preview_model);
	ui.preView->setHidden(true);

	ui.tabBar->addTab(QIcon(":/icons/icon_user.png"), "Пациенти");
	ui.tabBar->addTab(QIcon(":/icons/icon_sheet.png"), "Амбулаторни листове");
	ui.tabBar->addTab(QIcon(":/icons/icon_prescr.png"), "Рецепти");
	ui.tabBar->addTab(QIcon(":/icons/icon_periosheet.png"), "Пародонтални измервания");
	ui.tabBar->addTab(QIcon(":/icons/icon_invoice.png"), "Финансови документи");

	ui.tabBar->setExpanding(false);
	ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

	ui.frame->setStyleSheet("QFrame#frame{"
		"background-color: rgb(249,249,249);"
		"}");


	setWindowFlags(Qt::Window);
	setWindowTitle("Документация");
	setWindowIcon(QIcon(":/icons/icon_open.png"));

	auto lambda = [](const QDate& date) { return Date{ date.day(), date.month(), date.year() }; };

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
		[&](int idx) {
			presenter.setListType(static_cast<TabType>(idx));
			hideRanges(!idx);
		});

	connect(ui.idSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.idSearchEdit->text();
			idFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
			setCountLabel();
		});

	connect(ui.nameSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.nameSearchEdit->text();
			nameFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
			setCountLabel();
		});

	connect(ui.phoneSearchEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.phoneSearchEdit->text();
			phoneFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
			setCountLabel();
		});


	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.detailsCheck, &QCheckBox::toggled, [&](bool checked) { ui.preView->setHidden(!checked); });

	connect(ui.tableView, &QTableView::customContextMenuRequested, this, [=](const QPoint& p) {contextMenuRequested(p); });

	connect(ui.tableView, &QTableView::doubleClicked, this, [&] { presenter.openCurrentSelection(); });

	connect(ui.tableView, &ListTable::deletePressed, this, [=] { ui.deleteButton->click(); });

	connect(ui.deleteButton, &QPushButton::clicked, 
		[=] 
		{
				presenter.deleteCurrentSelection();
		});


	connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection& selected, const QItemSelection& deselected){

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			int index = phoneFilter.index(idx.row(), 0).data().toInt();
			selectedIndexes.insert(index);
		}

		presenter.selectionChanged(selectedIndexes);

		}

	);

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

void BrowserDialog::setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn)
{
	table_model.setData(t);

	for (int i = 0; i < t.size(); i++) {
		ui.tableView->setColumnWidth(i, t[i].width);
	}

	setCountLabel();

	idFilter.setFilterKeyColumn(idColumn);
	nameFilter.setFilterKeyColumn(nameColumn);
	phoneFilter.setFilterKeyColumn(phoneColumn);
	ui.tableView->hideColumn(0);
}

void BrowserDialog::setPreview(const PlainTable& t)
{
	preview_model.setData(t);
//	ui.preView->setHidden(!t.size());

	for (int i = 0; i < t.size(); i++) {
		ui.preView->setColumnWidth(i, t[i].width);
	}
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

	action = (new QAction("Копирай текста", main_menu));
	action->setIcon(QIcon(":/icons/icon_copy.png"));
	connect(action, &QAction::triggered, [=] { 

		QClipboard* clipboard = QApplication::clipboard();
		QString text = ui.tableView->currentIndex().data().toString();
		clipboard->setText(text);
	});
	main_menu->addAction(action);

	action = (new QAction("Изтрий", main_menu));
	connect(action, &QAction::triggered, [=] { presenter.deleteCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_remove.png"));
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

void BrowserDialog::setCountLabel()
{
	QString text = "Брой резултати: ";
	text += QString::number(phoneFilter.rowCount());
	ui.countLabel->setText(text);
}

void BrowserDialog::focus()
{
	activateWindow();
}

void BrowserDialog::close()
{
	QDialog::accept();
}

