#include "BrowserDialog.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMenu>

#include "View/Theme.h"
#include "View/ModalDialogBuilder.h"

#include "Presenter/BrowserPresenter.h"

BrowserDialog::BrowserDialog()
{
	ui.setupUi(this);

	ui.preView->horizontalHeader()->setStretchLastSection(true);

	idFilter.setSourceModel(&table_model);
	nameFilter.setSourceModel(&idFilter);
	phoneFilter.setSourceModel(&nameFilter);
	ui.tableView->setModel(&phoneFilter);
	ui.preView->setModel(&preview_model);

	ui.tabBar->addTab(QIcon(":/icons/icon_user.png"), "Пациенти");
	ui.tabBar->addTab(QIcon(":/icons/icon_sheet.png"), "Амбулаторни листове");
	ui.tabBar->addTab(QIcon(":/icons/icon_prescr.png"), "Рецепти");
	ui.tabBar->addTab(QIcon(":/icons/icon_periosheet.png"), "Пародонтални измервания");
	ui.tabBar->addTab(QIcon(":/icons/icon_invoice.png"), "Финансови документи");

	ui.openButton->setIcon(QIcon(":/icons/icon_open.png"));
	ui.deleteButton->setIcon(QIcon(":/icons/icon_remove.png"));

	ui.tabBar->setExpanding(false);
	ui.tabBar->setElideMode(Qt::TextElideMode::ElideNone);

	ui.frame->setStyleSheet("QFrame#frame{"
		"background-color: rgb(249,249,249);"
		"}");


	setWindowFlags(Qt::Window);
	setWindowTitle("Документация");
	setWindowIcon(QIcon(":/icons/icon_open.png"));

    connect(ui.fromDateEdit, &QDateEdit::dateChanged, this, [&]{ datesChanged(); });
    connect(ui.toDateEdit, &QDateEdit::dateChanged, this, [&]{ datesChanged(); });

    connect(ui.openButton, &QPushButton::clicked, this, [&] {

		QApplication::setOverrideCursor(QCursor(Qt::CursorShape::WaitCursor));
		presenter.openCurrentSelection();
		QApplication::restoreOverrideCursor();

		});

	connect(ui.tabBar, &QTabBar::currentChanged,
		[&](int idx) {
			presenter.setListType(static_cast<TabType>(idx));
			calculateUiState();
		});

    connect(ui.idSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui.idSearchEdit->text().replace(" ","");
			idFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
			setCountLabel();
		});

    connect(ui.nameSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui.nameSearchEdit->text();
			nameFilter.setName(text);
			setCountLabel();
		});

    connect(ui.phoneSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui.phoneSearchEdit->text().replace(" ", "");
			phoneFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
			setCountLabel();
		});


	ui.tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui.detailsCheck, &QCheckBox::toggled, this, [&](bool checked) {
			presenter.showDetailsPane(checked); 
			calculateUiState();
	
		});

    connect(ui.detailsCombo, &QComboBox::currentIndexChanged, this, [&](int idx) { presenter.showProcedureDetails(idx); calculateUiState(); });

    connect(ui.tableView, &QTableView::customContextMenuRequested, this, [&](const QPoint& p) { contextMenuRequested(p); });

	connect(ui.tableView, &QTableView::doubleClicked, this, [&] { presenter.openCurrentSelection(); });

    connect(ui.preView, &QTableView::doubleClicked, this, [&]{ emit ui.openDocButton->clicked(); });

    connect(ui.tableView, &ListTable::deletePressed, this, [&] { ui.deleteButton->click(); });

    connect(ui.deleteButton, &QPushButton::clicked, this, [&]{ presenter.deleteCurrentSelection(); });


    connect(ui.openDocButton, &QPushButton::clicked, this, [&] {

		auto idxList = ui.preView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			selectedIndexes.insert(idx.row());
		}

		presenter.openPatientDocuments(selectedIndexes);
	});

    connect(ui.tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&](const QItemSelection&, const QItemSelection&){

		auto idxList = ui.tableView->selectionModel()->selectedRows();

		std::set<int>selectedIndexes;

		for (auto& idx : idxList) {
			int index = phoneFilter.index(idx.row(), 0).data().toInt();
			selectedIndexes.insert(index);
		}

		presenter.selectionChanged(selectedIndexes);

	});

    connect(ui.preView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&](const QItemSelection&, const QItemSelection&){

		ui.openDocButton->setEnabled(ui.preView->selectionModel()->hasSelection());

	});

	ui.nameSearchEdit->setFocus();

	presenter.setView(this);

	//ui.dataTypeCombo->setCurrentIndex(2);

}

BrowserDialog::~BrowserDialog()
{
}

void BrowserDialog::datesChanged()
{
    auto lambda = [](const QDate& date) { return Date{ date.day(), date.month(), date.year() }; };

    presenter.setDates(lambda(ui.fromDateEdit->date()), lambda(ui.toDateEdit->date()));
}

void BrowserDialog::setUiState(const BrowserUiState& state)
{	
	QSignalBlocker blockers[5] = {
		QSignalBlocker(ui.fromDateEdit),
		QSignalBlocker(ui.toDateEdit),
		QSignalBlocker(ui.detailsCheck),
		QSignalBlocker(ui.detailsCombo),
		QSignalBlocker(ui.tabBar)
	};

	ui.fromDateEdit->setDate(QDate(state.from.year, state.from.month, state.from.day));
	ui.toDateEdit->setDate(QDate(state.to.year, state.to.month, state.to.day));
	ui.detailsCombo->setCurrentIndex(state.showProcedures);
	ui.detailsCheck->setChecked(state.showDetails);
	ui.tabBar->setCurrentIndex(static_cast<int>(state.model_type));
	calculateUiState();
	
}

void BrowserDialog::setTable(const PlainTable& t, int idColumn, int nameColumn, int phoneColumn)
{
    table_model.setTableData(t);

	for (int i = 0; i < t.size(); i++) {

		t[i].hidden ?
			ui.tableView->hideColumn(i)
			:
			ui.tableView->showColumn(i);

		ui.tableView->setColumnWidth(i, t[i].width);
		
	}

	setCountLabel();

	idFilter.setFilterKeyColumn(idColumn);
	nameFilter.setFilterKeyColumn(nameColumn);
	phoneFilter.setFilterKeyColumn(phoneColumn);
}

void BrowserDialog::setPreview(const PlainTable& t)
{
    preview_model.setTableData(t);

	//because selection model resets
	ui.openDocButton->setDisabled(true);

	for (int i = 0; i < t.size(); i++) {
		
		t[i].hidden ?
			ui.preView->hideColumn(i)
			:
			ui.preView->showColumn(i);

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
    connect(action, &QAction::triggered, this, [=, this] { presenter.openCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_open.png"));
	main_menu->addAction(action);


	if (ui.tabBar->currentIndex() != 4) {
		action = (new QAction("Нов амбулаторен лист", main_menu));
        connect(action, &QAction::triggered, this, [=, this] { presenter.openNewDocument(TabType::AmbList); });
		action->setIcon(QIcon(":/icons/icon_sheet.png"));
		main_menu->addAction(action);

		action = (new QAction("Ново пародонтално измерване", main_menu));
        connect(action, &QAction::triggered, this, [=, this] { presenter.openNewDocument(TabType::PerioStatus); });
		action->setIcon(QIcon(":/icons/icon_periosheet.png"));
		main_menu->addAction(action);

		action = (new QAction("Нова рецепта", main_menu));
        connect(action, &QAction::triggered, this, [=, this] { presenter.openNewDocument(TabType::Prescription); });
		action->setIcon(QIcon(":/icons/icon_prescr.png"));
		main_menu->addAction(action);

		action = (new QAction("Нова фактура", main_menu));
        connect(action, &QAction::triggered, this, [=, this] { presenter.openNewDocument(TabType::Financial); });
		action->setIcon(QIcon(":/icons/icon_invoice.png"));
		main_menu->addAction(action);
	}

	action = (new QAction("Копирай текста", main_menu));
	action->setIcon(QIcon(":/icons/icon_copy.png"));
    connect(action, &QAction::triggered, this, [=, this] {

		QClipboard* clipboard = QApplication::clipboard();
		QString text = ui.tableView->currentIndex().data().toString();
		clipboard->setText(text);
	});
	main_menu->addAction(action);

	action = (new QAction("Изтрий", main_menu));
    connect(action, &QAction::triggered, this, [=, this] { presenter.deleteCurrentSelection(); });
	action->setIcon(QIcon(":/icons/icon_remove.png"));
	main_menu->addAction(action);

	main_menu->setStyleSheet(Theme::getPopupMenuStylesheet());

	main_menu->popup(ui.tableView->viewport()->mapToGlobal(p));
}

void BrowserDialog::setCountLabel()
{
	QString text = "Брой резултати: ";
	text += QString::number(phoneFilter.rowCount());
	ui.countLabel->setText(text);
}

void BrowserDialog::calculateUiState()
{
	bool patientSection = ui.tabBar->currentIndex() == 0;
	bool hideDetailsCombo = !(ui.detailsCheck->isChecked() && patientSection);
	bool hideOpenDocButton = !(ui.detailsCheck->isChecked() && patientSection && !ui.detailsCombo->currentIndex());

	ui.preView->setHidden(!ui.detailsCheck->isChecked());
	ui.detailsCombo->setHidden(hideDetailsCombo);
	ui.openDocButton->setHidden(hideOpenDocButton);
	ui.fromLabel->setHidden(patientSection);
	ui.toLabel->setHidden(patientSection);
	ui.fromDateEdit->setHidden(patientSection);
	ui.toDateEdit->setHidden(patientSection);
	
}

void BrowserDialog::focus()
{
	activateWindow();
}

void BrowserDialog::close()
{
	QDialog::accept();
}

bool NameFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	auto str =  sourceModel()->index(sourceRow, filterKeyColumn()).data().toString().toLower();

	for (auto& name : m_names) {
		if (!str.contains(name)) return false;
	}

	return true;
}

void NameFilterProxyModel::setName(const QString& name)
{
	m_names = name.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
	
	for (auto& name : m_names) {
		name = name.toLower();
	}

	invalidateRowsFilter();
}
