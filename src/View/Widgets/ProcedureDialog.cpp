#include "ProcedureDialog.h"

#include "Presenter/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Добавяне на манипулация");

	ui.stackedWidget->setCurrentIndex(0);
	ui.commonFields->hide();

	auto table = ui.tableView;

	ui.commonFields->setExternalDateEdit(ui.dateEdit);
	ui.dateEdit->setErrorLabel(ui.errorLabel);

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(2);
	table->setModel(&proxyModel);

	table->hideColumn(0);
	table->setColumnWidth(0, 20);
	table->setColumnWidth(1, 70);
	table->setColumnWidth(3, 70);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);

	connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [=] {
	
		s_idx = table->selectionModel()->currentIndex().row();

		if (s_idx == -1){
			
			presenter->indexChanged(s_idx);
			return;
		}

		int procedure = proxyModel.index(s_idx, 0).data().toInt();
		presenter->indexChanged(procedure);
		});

	connect(ui.cancelButton, &QPushButton::clicked, [=] { close(); });
	connect(ui.okButton, &QPushButton::clicked, [=] { presenter->formAccepted(); });

	connect(ui.searchEdit, &QLineEdit::textChanged, [=]
		{
			s_search = ui.searchEdit->text();
			proxyModel.setFilterRegularExpression(QRegularExpression(s_search, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(s_idx);
		});


	connect(ui.tableView, &QTableView::doubleClicked, [=] { presenter->formAccepted(); });

	connect(ui.dateEdit, &QDateEdit::dateChanged, [=] {

		auto date = ui.dateEdit->date();
		presenter->procedureDateChanged(Date{ date.day(), date.month(), date.year() });
		presenter->indexChanged(-1);
		});

	ui.crownWidget->ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.obturWidget->ui.surfaceSelector->setErrorLabel(ui.errorLabel);
	ui.fiberWidget->ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.diagnosisEdit->setErrorLabel(ui.errorLabel);
	ui.commonFields->ui.manipulationEdit->setErrorLabel(ui.errorLabel);
	


	ui.searchEdit->setText(s_search);
	ui.tableView->selectRow(s_idx);


	presenter->setView(this);


}

ProcedureDialog::~ProcedureDialog()
{
}


void ProcedureDialog::setProcedureTemplates(std::vector<ProcedureTemplate> procedureList)
{
	model.setProcedures(procedureList);
}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString selectedTeeth;

	selectedTeeth.reserve(15 + selectedTeethNum.size() * 3);

	selectedTeeth.append("Избрани зъби: ");

	for (int i = 0; i < selectedTeethNum.size(); i++)
	{
		selectedTeeth.append(QString::number(selectedTeethNum[i]));
		if (i < selectedTeethNum.size() - 1)
		{
			selectedTeeth.append(", ");
		}
	}

	if (!selectedTeethNum.size())
	{
		selectedTeeth.append("Няма");
	}

	ui.statusLabel->setText(selectedTeeth);
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

void ProcedureDialog::setFiberSplintPresenter(FiberSplintPresenter* presenter)
{
	presenter->setCommonFieldsView(ui.commonFields);
	presenter->setView(ui.fiberWidget);
	ui.fiberWidget->setPresenter(presenter);
}

ICommonFields* ProcedureDialog::commonFields()
{
	return ui.commonFields;
}

void ProcedureDialog::setView(ProcedureTemplateType t)
{

	ui.commonFields->show();
	ui.errorLabel->show();

	switch (t)
	{	
	case ProcedureTemplateType::obturation:
		ui.stackedWidget->setCurrentWidget(ui.obturWidget);
		break;
	case ProcedureTemplateType::extraction:
		ui.stackedWidget->setCurrentIndex(0);
		break;
	case ProcedureTemplateType::prosthodontic:
		ui.stackedWidget->setCurrentWidget(ui.crownWidget);
		break;
	case ProcedureTemplateType::implant:
		ui.stackedWidget->setCurrentWidget(ui.implantWidget);
		break;
	case ProcedureTemplateType::fibersplint:
		ui.stackedWidget->setCurrentWidget(ui.fiberWidget);
		break;
	default:
		ui.stackedWidget->setCurrentIndex(0);
		ui.selectToothLabel->setText("");
	}


}

void ProcedureDialog::close() { QDialog::accept(); }

void ProcedureDialog::showErrorMessage(const std::string& error)
{
	ui.commonFields->hide();
	ui.errorLabel->hide();
	ui.stackedWidget->setCurrentIndex(0);
	ui.selectToothLabel->setText(QString::fromStdString(error));
}

