#include "ProcedureDialog.h"
//#include <QPainter>

#include "Presenter/ProcedureDialog/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Добавяне на манипулация");

	auto table = ui.tableView;

	//proxyModel = new QSortFilterProxyModel(this);

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(2);
	presenter->setView(this);

	table->setModel(&proxyModel);
	table->hideColumn(0);
	table->setColumnWidth(0, 20);
	table->setColumnWidth(1, 70);
	table->setColumnWidth(2, 370);
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

		int manipulationIdx = proxyModel.index(row, 0).data().toInt();
		presenter->indexChanged(manipulationIdx);
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { presenter->formAccepted(); });

	connect(ui.searchEdit, &QLineEdit::textChanged, [=]
		{
			proxyModel.setFilterRegExp(QRegExp(ui.searchEdit->text(), Qt::CaseInsensitive, QRegExp::FixedString));
			ui.tableView->selectRow(0);
		});


	connect(ui.tableView, &QTableView::doubleClicked, [=] { presenter->formAccepted(); });

	ui.errorLabel->setStyleSheet("color:red");
	ui.crownWidget->ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.obturWidget->ui.surfaceSelector->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.diagnosisEdit->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.manipulationEdit->setErrorLabel(ui.errorLabel);
}

ProcedureDialog::~ProcedureDialog()
{
}


void ProcedureDialog::loadManipulationList(std::vector<ProcedureTemplate> procedureList)
{
	model.setProcedures(procedureList);
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

void ProcedureDialog::setImplantPresenter(ImplantPresenter* presenter)
{
	presenter->setCommonFieldsView(ui.commonFields);
	presenter->setView(ui.implantWidget);
	ui.implantWidget->setPresenter(presenter);
}

ICommonFields* ProcedureDialog::commonFields()
{
	return ui.commonFields;
}


void ProcedureDialog::resetForm()
{
	ui.commonFields->hide();
	ui.stackedWidget->setCurrentIndex(0);

}

void ProcedureDialog::setView(ProcedureType t)
{
	resetForm();

	ui.commonFields->show();

	switch (t)
	{	
	case ProcedureType::obturation:
		ui.stackedWidget->setCurrentIndex(1);
		break;
	case ProcedureType::extraction:
		break;
	case ProcedureType::crown:
		ui.stackedWidget->setCurrentIndex(2);
		break;
	case ProcedureType::implant:
		ui.stackedWidget->setCurrentIndex(3);
		break;
	default:
		ui.stackedWidget->setCurrentIndex(0);
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

