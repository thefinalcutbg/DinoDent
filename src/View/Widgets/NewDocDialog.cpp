#include "NewDocDialog.h"
#include "ui_NewDocDialog.h"
#include "Presenter/NewDocPresenter.h"
#include <QKeyEvent>

NewDocDialog::NewDocDialog(NewDocPresenter& presenter)
	: QDialog(nullptr)
	, ui(new Ui::NewDocDialogClass())
{
	ui->setupUi(this);

    ui->idSearchEdit->installEventFilter(this);
    ui->nameSearchEdit->installEventFilter(this);
    ui->phoneSearchEdit->installEventFilter(this);

	idFilter.setSourceModel(&table_model);
	nameFilter.setSourceModel(&idFilter);
	phoneFilter.setSourceModel(&nameFilter);
	ui->tableView->setModel(&phoneFilter);

	ui->newPatient->setIcon(QIcon(":/icons/icon_add.png"));
	ui->nameSearchEdit->setFocus();

    connect(ui->idSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui->idSearchEdit->text().replace(" ", "");
			idFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		
			ui->tableView->selectRow(0);
		});

	connect(ui->nameSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui->nameSearchEdit->text();
			nameFilter.setName(text);

			ui->tableView->selectRow(0);
		});

	connect(ui->phoneSearchEdit, &QLineEdit::textChanged, this, [&]
		{
			QString text = ui->phoneSearchEdit->text().replace(" ", "");
			phoneFilter.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
		
			ui->tableView->selectRow(0);
		});

	connect(ui->okButton, &QPushButton::clicked, this, [&] {

		if (!ui->tableView->model()->rowCount()) {
			presenter.newPatient();
			return;
		}

		auto idxList = ui->tableView->selectionModel()->selectedRows();

		if (idxList.empty()) {
			return;
		}

		for (auto& idx : idxList) {
			long long rowid = phoneFilter.index(idx.row(), 0).data().toLongLong();
			presenter.patientSelected(rowid);
		}
		
	});

	connect(ui->tableView, &QTableView::doubleClicked, this, [&] {
		ui->okButton->clicked();
	});

	connect(ui->newPatient, &QPushButton::clicked, this, [&] {
		presenter.newPatient();
	});


}

void NewDocDialog::setTable(const PlainTable& t)
{
	table_model.setTableData(t);

	for (int i = 0; i < t.size(); i++) {

		t[i].hidden ?
			ui->tableView->hideColumn(i)
			:
			ui->tableView->showColumn(i);

		ui->tableView->setColumnWidth(i, t[i].width);

	}

	idFilter.setFilterKeyColumn(1);
	nameFilter.setFilterKeyColumn(2);
	phoneFilter.setFilterKeyColumn(3);

	ui->tableView->selectRow(0);
	ui->tableView->horizontalHeader()->setStretchLastSection(false);
	ui->tableView->setColumnWidth(3, 150);
	ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
}

bool NewDocDialog::eventFilter(QObject* obj, QEvent* e)
{

	if (
		e->type() == QEvent::KeyPress && 
        (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Down ||
		static_cast<QKeyEvent*>(e)->key() == Qt::Key_Up
		)
	)
	{
		int currentRow = ui->tableView->currentIndex().row();

		currentRow +=
			static_cast<QKeyEvent*>(e)->key() == Qt::Key_Up ?
			-1 : 1;

		ui->tableView->selectRow(currentRow);

        return true;

	}

	return QObject::eventFilter(obj, e);

}

NewDocDialog::~NewDocDialog()
{
	delete ui;
}
