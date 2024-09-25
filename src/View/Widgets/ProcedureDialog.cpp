#include "ProcedureDialog.h"
#include "Presenter/ProcedureDialogPresenter.h"

ProcedureDialog::ProcedureDialog(ProcedureDialogPresenter& presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter), proxyModel(this)
{
	ui.setupUi(this);

	setModal(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Добавяне на манипулация");

	proxyModel.setSourceModel(&model);
	proxyModel.setFilterKeyColumn(2);

	auto& table = ui.tableView;

	table->setModel(&proxyModel);
	table->setMouseTracking(true);
	table->setColumnWidth(0, 30);
	table->setColumnWidth(1, 70);
	table->hideColumn(3);
	//table->setColumnWidth(3, 70);
	table->setSelectionBehavior(QAbstractItemView::SelectRows);
	table->setSelectionMode(QAbstractItemView::SingleSelection);
	table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);

	table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	table->verticalHeader()->hide();

	auto favDelagete = new FavButtonDelegate();
	table->setItemDelegateForColumn(0, favDelagete);

	connect(favDelagete, &FavButtonDelegate::updateRequested, this, [&] {
		ui.tableView->viewport()->update();
	});

	connect(favDelagete, &FavButtonDelegate::favouriteClicked, this, [&](int rowIdx) {
		presenter.favouriteClicked(rowIdx);
		
	});

    connect(table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [&] {

            s_idx = table->selectionModel()->currentIndex().row();

            if (s_idx == -1){
                presenter.indexChanged(s_idx);
				ui.tableView->clearSelection(); //because of bug when reseting the model
                return;
            }

            int procedure = proxyModel.index(s_idx, 0).data().toInt();
            presenter.indexChanged(procedure);
		});

    connect(ui.cancelButton, &QPushButton::clicked, this, [&] { close(); });
    connect(ui.okButton, &QPushButton::clicked, this, [&] { presenter.formAccepted(); });

    connect(ui.searchEdit, &QLineEdit::textChanged, this, [=, this]
		{
			s_search = ui.searchEdit->text();
			proxyModel.setFilterRegularExpression(QRegularExpression(s_search, QRegularExpression::CaseInsensitiveOption));
			ui.tableView->selectRow(s_idx);
		});


    connect(ui.tableView, &QTableView::doubleClicked, this, [&] { presenter.formAccepted(); });

    connect(ui.procedureInput->qDateEdit(), &QDateEdit::dateChanged, this, [&] {

		auto date = ui.procedureInput->qDateEdit()->date();
        presenter.procedureDateChanged(Date{ date.day(), date.month(), date.year() });
        
	});

	
	ui.searchEdit->setText(s_search);
	ui.tableView->selectRow(s_idx);

    presenter.setView(this);


}

ProcedureDialog::~ProcedureDialog()
{
}


void ProcedureDialog::setProcedureTemplates(std::vector<ProcedureListElement> procedureList)
{
	model.setProcedures(procedureList);	
	presenter.indexChanged(-1);

}

void ProcedureDialog::setSelectionLabel(const std::vector<int>& selectedTeethNum)
{
	QString selectedTeeth;

	selectedTeeth.reserve(15 + selectedTeethNum.size() * 3);

	selectedTeeth.append("Избрани зъби: ");

    for (size_t i = 0; i < selectedTeethNum.size(); i++)
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

IProcedureInput* ProcedureDialog::procedureInput()
{
	return ui.procedureInput;
}


void ProcedureDialog::close() { QDialog::accept(); }



