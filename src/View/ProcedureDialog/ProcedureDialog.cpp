#include "ProcedureDialog.h"
#include <QPainter>

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter)
{
	ui.setupUi(this);
	
	setWindowTitle("Добавяне на манипулация");

	auto table = ui.tableView;

	proxyModel = new QSortFilterProxyModel(this);

	proxyModel->setSourceModel(&model);
	proxyModel->setFilterKeyColumn(2);
	presenter->setView(this);

	table->setModel(proxyModel);
	table->hideColumn(0);
	table->setColumnWidth(0, 20);
	table->setColumnWidth(1, 20);
	table->setColumnWidth(2, 400);
	table->setColumnWidth(3, 70);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);


	connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=] {
	
		int row =  table->selectionModel()->currentIndex().row();

		if (row == -1){
			
			presenter->indexChanged(row);
			return;
		}

		int manipulationIdx = proxyModel->index(row, 0).data().toInt();
		presenter->indexChanged(manipulationIdx);
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { presenter->formAccepted(); });

	connect(ui.searchEdit, &QLineEdit::textChanged, [=]
		{
			proxyModel->setFilterRegExp(QRegExp(ui.searchEdit->text(), Qt::CaseInsensitive, QRegExp::FixedString));
			ui.tableView->selectRow(0);
		});


	connect(ui.tableView, &QTableView::doubleClicked, [=] { presenter->formAccepted(); });
}

ProcedureDialog::~ProcedureDialog()
{
}

void ProcedureDialog::openProcedureDialog()
{
	ui.searchEdit->setText("");
	ui.searchEdit->setFocus();
	ui.tableView->selectRow(0);
	exec();

}

void ProcedureDialog::loadManipulationList(std::vector<ManipulationTemplate> manipulationList)
{
	qDebug() << "Loading manipulations: " << manipulationList.size();
	model.setManipulations(manipulationList);
}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString label;

	label.reserve(15 + selectedTeethNum.size() * 3);

	label.append("Избрани зъби: ");

	for (int i = 0; i < selectedTeethNum.size(); i++)
	{
		label.append(QString::number(selectedTeethNum[i]));
		if (i < selectedTeethNum.size() - 1)
		{
			label.append(", ");
		}
	}

	if (!selectedTeethNum.size())
	{
		label.append("Няма");
	}

	ui.selectedTeethLabel->setText(label);
}

void ProcedureDialog::setObturationPresenter(ObturationPresenter* presenter)
{
	presenter->setCommonFieldsView(ui.commonFields);
	presenter->setView(ui.obturWidget);
	ui.obturWidget->setPresenter(presenter);
}

void ProcedureDialog::setCrownPresenter(CrownPresenter* presenter)
{
	presenter->setCommonFieldsView(ui.commonFields);
	presenter->setView(ui.crownWidget);
	ui.crownWidget->setPresenter(presenter);
}

ICommonFields* ProcedureDialog::commonFields()
{
	return ui.commonFields;
}

void ProcedureDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter;
	painter.begin(this);
	painter.fillRect(0, 0, width(), height(), Qt::white);
	painter.end();
}

void ProcedureDialog::resetForm()
{
	ui.commonFields->hide();
	ui.obturWidget->hide();
	ui.crownWidget->hide();

}

void ProcedureDialog::setView(ManipulationType t)
{
	resetForm();

	ui.commonFields->show();

	switch (t)
	{	
	case ManipulationType::obturation:
		ui.obturWidget->show();
		break;
	case ManipulationType::extraction:
		break;
	case ManipulationType::crown:
		ui.crownWidget->show();
		break;
	}
}

void ProcedureDialog::close()
{
	this->hide();
}

void ProcedureDialog::showErrorMessage(const std::string& error)
{
	resetForm();
	ui.noTeethLabel->show();
	ui.noTeethLabel->setText(QString::fromStdString(error));
}

void ProcedureDialog::showErrorDialog(const std::string& error)
{
	QMessageBox m;
	m.setText(QString::fromStdString(error));
	m.setIcon(QMessageBox::Warning);
	m.exec();
}

