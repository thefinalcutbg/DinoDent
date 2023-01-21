#include "ProcedureDialog.h"

#include "Presenter/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Добавяне на манипулация");

	auto table = ui.tableView;

	//ui.commonFields->setExternalDateEdit(ui.dateEdit);
	//ui.dateEdit->setErrorLabel(ui.errorLabel);

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(2);
	table->setModel(&proxyModel);

	table->hideColumn(0);
	table->setColumnWidth(0, 20);
	table->setColumnWidth(1, 70);
	table->hideColumn(3);
	//table->setColumnWidth(3, 70);
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

	connect(ui.commonFields->qDateEdit(), &QDateEdit::dateChanged, [=] {

		auto date = ui.commonFields->qDateEdit()->date();
		presenter->procedureDateChanged(Date{ date.day(), date.month(), date.year() });
		presenter->indexChanged(-1);
		});

	
	


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

ICommonFields* ProcedureDialog::commonFields()
{
	return ui.commonFields;
}


void ProcedureDialog::close() { QDialog::accept(); }



